// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/EverMemOSSubsystem.h"
#include "EverMemOS/EverMemOSSettings.h"
#include "EverMemOS/EverMemOSLog.h"
#include "EverMemOS/Auth/EverMemOSBearerAuth.h"
#include "EverMemOS/Auth/EverMemOSHMACAuth.h"
#include "EverMemOS/Auth/EverMemOSNoAuth.h"
#include "EverMemOS/Http/EverMemOSHttpClient.h"
#include "EverMemOS/Blueprint/EverMemOSBlueprintLibrary.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Async/Async.h"

namespace
{
FString ToJsonArrayParam(const TArray<FString>& Values)
{
	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	Writer->WriteArrayStart();
	for (const FString& Value : Values)
	{
		Writer->WriteValue(Value);
	}
	Writer->WriteArrayEnd();
	Writer->Close();
	return Json;
}

void ParseBestEffortMetadata(const TSharedPtr<FJsonObject>& MetaObj, TMap<FString, FString>& OutMetadata)
{
	if (!MetaObj.IsValid())
	{
		return;
	}

	for (const auto& Pair : MetaObj->Values)
	{
		FString ValString;
		if (Pair.Value.IsValid() && Pair.Value->TryGetString(ValString))
		{
			OutMetadata.Add(Pair.Key, ValString);
		}
	}
}

void ParseMemoryRecords(const TArray<TSharedPtr<FJsonValue>>& MemoriesArray, TArray<FEverMemOSMemoryRecord>& OutMemories)
{
	for (const TSharedPtr<FJsonValue>& MemVal : MemoriesArray)
	{
		const TSharedPtr<FJsonObject>* MemObj = nullptr;
		if (!MemVal.IsValid() || !MemVal->TryGetObject(MemObj) || !MemObj || !(*MemObj).IsValid())
		{
			continue;
		}

		FEverMemOSMemoryRecord Memory;
		(*MemObj)->TryGetStringField(TEXT("Id"), Memory.Id);
		(*MemObj)->TryGetStringField(TEXT("UserId"), Memory.UserId);
		(*MemObj)->TryGetStringField(TEXT("GroupId"), Memory.GroupId);
		(*MemObj)->TryGetStringField(TEXT("MemoryType"), Memory.MemoryType);
		(*MemObj)->TryGetStringField(TEXT("Summary"), Memory.Summary);
		(*MemObj)->TryGetStringField(TEXT("Content"), Memory.Content);
		(*MemObj)->TryGetStringField(TEXT("Timestamp"), Memory.Timestamp);

		OutMemories.Add(Memory);
	}
}
}

void UEverMemOSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	HttpClient = MakeShared<FEverMemOSHttpClient>();
	RebuildClient();

	// Listen for settings changes
	UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>();
	if (Settings)
	{
		SettingsChangedHandle = Settings->OnSettingsChanged.AddUObject(this, &UEverMemOSSubsystem::OnSettingsChanged);
	}

	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Subsystem initialized"));
}

void UEverMemOSSubsystem::Deinitialize()
{
	bIsShuttingDown = true;

	// Cancel all active HTTP requests
	if (HttpClient.IsValid())
	{
		HttpClient->CancelAllRequests();
	}

	// Unsubscribe from settings changes
	UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>();
	if (Settings)
	{
		Settings->OnSettingsChanged.Remove(SettingsChangedHandle);
	}

	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Subsystem deinitialized"));

	Super::Deinitialize();
}

void UEverMemOSSubsystem::RebuildClient()
{
	const UEverMemOSSettings* Settings = GetDefault<UEverMemOSSettings>();
	if (!Settings)
	{
		UE_LOG(LogEverMemOS, Error, TEXT("Failed to get EverMemOS settings"));
		return;
	}

	TSharedPtr<IEverMemOSAuthProvider> Auth = CreateAuthProvider();
	FEverMemOSRetryPolicy Policy(Settings->MaxRetries, Settings->RetryDelaySeconds);
	const FString StatusPath = UEverMemOSSettings::GetStatusPathSegment(Settings->DeploymentProfile);

	HttpClient->Configure(Settings->BaseURL, Settings->ApiVersion,
		StatusPath, Settings->TimeoutSeconds, Auth, Policy);

	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS client configured: %s/api/%s (profile=%s)"),
			*Settings->BaseURL, *Settings->ApiVersion,
			Settings->DeploymentProfile == EEverMemOSDeploymentProfile::Local ? TEXT("Local") : TEXT("Cloud"));
}

void UEverMemOSSubsystem::SetRuntimeBearerToken(const FString& Token)
{
	RuntimeBearerToken = Token;
	bHasRuntimeBearerToken = !Token.IsEmpty();
	RebuildClient();
}

void UEverMemOSSubsystem::SetRuntimeHMACSecret(const FString& Secret)
{
	RuntimeHMACSecret = Secret;
	bHasRuntimeHMACSecret = !Secret.IsEmpty();
	RebuildClient();
}

void UEverMemOSSubsystem::OnSettingsChanged()
{
	if (!bIsShuttingDown)
	{
		RebuildClient();
	}
}

TSharedPtr<IEverMemOSAuthProvider> UEverMemOSSubsystem::CreateAuthProvider() const
{
	const UEverMemOSSettings* Settings = GetDefault<UEverMemOSSettings>();
	if (!Settings)
	{
		return nullptr;
	}

	// Local deployment does not require authentication
	if (!UEverMemOSSettings::RequiresAuth(Settings->DeploymentProfile))
	{
		return MakeShared<FEverMemOSNoAuth>();
	}

	const FString EffectiveBearer = bHasRuntimeBearerToken ? RuntimeBearerToken : Settings->BearerToken;
	const FString EffectiveHMAC = bHasRuntimeHMACSecret ? RuntimeHMACSecret : Settings->HMACSecretKey;

	switch (Settings->AuthMethod)
	{
	case EEverMemOSAuthMethod::Bearer:
		return MakeShared<FEverMemOSBearerAuth>(EffectiveBearer);
	case EEverMemOSAuthMethod::HMAC:
		return MakeShared<FEverMemOSHMACAuth>(EffectiveHMAC);
	default:
		return MakeShared<FEverMemOSBearerAuth>(EffectiveBearer);
	}
}

// --- Health Check ---

void UEverMemOSSubsystem::HealthCheck(FOnHealthCheckComplete OnComplete)
{
	if (bIsShuttingDown) return;

	HttpClient->GetRaw(TEXT("/health"), EEverMemOSResponseMode::Bare,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSHealthResponse(), Error);
					return;
				}

				FEverMemOSHealthResponse Response;
				if (Result.IsValid())
				{
					Result->TryGetStringField(TEXT("Message"), Response.Message);
					Result->TryGetStringField(TEXT("Service"), Response.Service);
					Response.Status = Response.Message.IsEmpty() ? TEXT("") : TEXT("ok");
				}

				OnComplete.ExecuteIfBound(Response, FEverMemOSError::NoError());
			}));
}

// --- Core Memory: Add ---

void UEverMemOSSubsystem::AddMemory(const FEverMemOSAddMemoryRequest& Request, FOnAddMemoryComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("MessageId"), Request.MessageId);
	Body->SetStringField(TEXT("CreateTime"), Request.CreateTime);
	Body->SetStringField(TEXT("Sender"), Request.Sender);
	Body->SetStringField(TEXT("Content"), Request.Content);

	if (!Request.GroupId.IsEmpty()) Body->SetStringField(TEXT("GroupId"), Request.GroupId);
	if (!Request.GroupName.IsEmpty()) Body->SetStringField(TEXT("GroupName"), Request.GroupName);
	if (!Request.SenderName.IsEmpty()) Body->SetStringField(TEXT("SenderName"), Request.SenderName);
	if (!Request.Role.IsEmpty()) Body->SetStringField(TEXT("Role"), Request.Role);

	if (Request.ReferList.Num() > 0)
	{
		TArray<TSharedPtr<FJsonValue>> ReferArray;
		ReferArray.Reserve(Request.ReferList.Num());
		for (const FString& Item : Request.ReferList)
		{
			ReferArray.Add(MakeShareable(new FJsonValueString(Item)));
		}
		Body->SetArrayField(TEXT("ReferList"), ReferArray);
	}

	Body->SetBoolField(TEXT("Flush"), Request.bFlush);

	// Add Memories uses the standard {status,message,result:{request_id}} envelope.
	HttpClient->Post(TEXT("/memories"), Body, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSAddMemoryResponse(), Error);
					return;
				}

				FEverMemOSAddMemoryResponse Response;
				if (Result.IsValid())
				{
					Result->TryGetStringField(TEXT("Status"), Response.Status);
					Result->TryGetStringField(TEXT("Message"), Response.Message);

					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetStringField(TEXT("RequestId"), Response.RequestId);
					}
				}

				OnComplete.ExecuteIfBound(Response, FEverMemOSError::NoError());
			}));
}

// --- Memorize ---

void UEverMemOSSubsystem::Memorize(const FEverMemOSMemorizeRequest& Request, FOnMemorizeComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("UserId"), Request.UserId);
	Body->SetStringField(TEXT("SessionId"), Request.SessionId);
	Body->SetStringField(TEXT("SceneType"), UEverMemOSBlueprintLibrary::SceneTypeToString(Request.SceneType));

	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const FEverMemOSMessage& Msg : Request.Messages)
	{
		TSharedPtr<FJsonObject> MsgObj = MakeShareable(new FJsonObject());
		MsgObj->SetStringField(TEXT("Role"), UEverMemOSBlueprintLibrary::MessageRoleToString(Msg.Role));
		MsgObj->SetStringField(TEXT("Content"), Msg.Content);
		MessagesArray.Add(MakeShareable(new FJsonValueObject(MsgObj)));
	}
	Body->SetArrayField(TEXT("Messages"), MessagesArray);

	HttpClient->Post(TEXT("/memories"), Body, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSMemorizeResult(), Error);
					return;
				}

				FEverMemOSMemorizeResult MemResult;
				if (Result.IsValid())
				{
					Result->TryGetStringField(TEXT("Status"), MemResult.Status);
					Result->TryGetStringField(TEXT("Message"), MemResult.Message);
				}

				OnComplete.ExecuteIfBound(MemResult, FEverMemOSError::NoError());
			}));
}

// --- Search ---

void UEverMemOSSubsystem::Search(const FEverMemOSSearchParams& Params, FOnSearchComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("user_id"), Params.UserId.IsEmpty() ? TEXT("__all__") : Params.UserId);
	if (Params.GroupIds.Num() > 0)
	{
		QueryParams.Add(TEXT("group_ids"), ToJsonArrayParam(Params.GroupIds));
	}
	QueryParams.Add(TEXT("query"), Params.Query);
	QueryParams.Add(TEXT("retrieve_method"), UEverMemOSBlueprintLibrary::RetrieveMethodToString(Params.RetrieveMethod));

	if (Params.MemoryTypes.Num() > 0)
	{
		TArray<FString> Types;
		Types.Reserve(Params.MemoryTypes.Num());
		for (EEverMemOSMemoryType Type : Params.MemoryTypes)
		{
			Types.Add(UEverMemOSBlueprintLibrary::MemoryTypeToString(Type));
		}
		QueryParams.Add(TEXT("memory_types"), ToJsonArrayParam(Types));
	}

	if (Params.bIncludeMetadata)
	{
		QueryParams.Add(TEXT("include_metadata"), TEXT("true"));
	}
	if (!Params.StartTime.IsEmpty()) QueryParams.Add(TEXT("start_time"), Params.StartTime);
	if (!Params.EndTime.IsEmpty()) QueryParams.Add(TEXT("end_time"), Params.EndTime);
	if (!Params.CurrentTime.IsEmpty()) QueryParams.Add(TEXT("current_time"), Params.CurrentTime);
	if (Params.Radius > 0) QueryParams.Add(TEXT("radius"), FString::FromInt(Params.Radius));

	if (Params.TopK >= 0) QueryParams.Add(TEXT("top_k"), FString::FromInt(Params.TopK));

	HttpClient->Get(TEXT("/memories/search"), QueryParams, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSSearchResult(), Error);
					return;
				}

				FEverMemOSSearchResult SearchResult;

				if (Result.IsValid())
				{
					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetNumberField(TEXT("TotalCount"), SearchResult.TotalCount);

						const TSharedPtr<FJsonObject>* MetaObj = nullptr;
						if ((*ResultObj)->TryGetObjectField(TEXT("Metadata"), MetaObj) && MetaObj)
						{
							ParseBestEffortMetadata(*MetaObj, SearchResult.Metadata);
						}

						const TArray<TSharedPtr<FJsonValue>>* MemoriesArray = nullptr;
						if ((*ResultObj)->TryGetArrayField(TEXT("Memories"), MemoriesArray) && MemoriesArray)
						{
							ParseMemoryRecords(*MemoriesArray, SearchResult.Memories);
						}
					}
				}

				OnComplete.ExecuteIfBound(SearchResult, FEverMemOSError::NoError());
			}));
}

// --- Core Memory: Get ---

void UEverMemOSSubsystem::GetMemories(const FEverMemOSGetMemoriesParams& Params, FOnGetMemoriesComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("user_id"), Params.UserId.IsEmpty() ? TEXT("__all__") : Params.UserId);
	if (Params.GroupIds.Num() > 0)
	{
		QueryParams.Add(TEXT("group_ids"), ToJsonArrayParam(Params.GroupIds));
	}
	if (!Params.MemoryType.IsEmpty()) QueryParams.Add(TEXT("memory_type"), Params.MemoryType);
	if (!Params.StartTime.IsEmpty()) QueryParams.Add(TEXT("start_time"), Params.StartTime);
	if (!Params.EndTime.IsEmpty()) QueryParams.Add(TEXT("end_time"), Params.EndTime);
	QueryParams.Add(TEXT("page"), FString::FromInt(Params.Page));
	QueryParams.Add(TEXT("page_size"), FString::FromInt(Params.PageSize));

	HttpClient->Get(TEXT("/memories"), QueryParams, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSGetMemoriesResult(), Error);
					return;
				}

				FEverMemOSGetMemoriesResult Out;

				if (Result.IsValid())
				{
					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetNumberField(TEXT("Count"), Out.Count);
						(*ResultObj)->TryGetNumberField(TEXT("TotalCount"), Out.TotalCount);

						const TSharedPtr<FJsonObject>* MetaObj = nullptr;
						if ((*ResultObj)->TryGetObjectField(TEXT("Metadata"), MetaObj) && MetaObj)
						{
							ParseBestEffortMetadata(*MetaObj, Out.Metadata);
						}

						const TArray<TSharedPtr<FJsonValue>>* MemoriesArray = nullptr;
						if ((*ResultObj)->TryGetArrayField(TEXT("Memories"), MemoriesArray) && MemoriesArray)
						{
							ParseMemoryRecords(*MemoriesArray, Out.Memories);
						}
					}
				}

				OnComplete.ExecuteIfBound(Out, FEverMemOSError::NoError());
			}));
}

// --- Core Memory: Delete ---

void UEverMemOSSubsystem::DeleteMemories(const FEverMemOSDeleteMemoriesRequest& Request, FOnDeleteMemoriesComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	if (!Request.MemoryId.IsEmpty()) Body->SetStringField(TEXT("MemoryId"), Request.MemoryId);
	if (!Request.Id.IsEmpty()) Body->SetStringField(TEXT("Id"), Request.Id);
	if (!Request.EventId.IsEmpty()) Body->SetStringField(TEXT("EventId"), Request.EventId);

	Body->SetStringField(TEXT("UserId"), Request.UserId.IsEmpty() ? TEXT("__all__") : Request.UserId);
	Body->SetStringField(TEXT("GroupId"), Request.GroupId.IsEmpty() ? TEXT("__all__") : Request.GroupId);

	HttpClient->Delete(TEXT("/memories"), Body, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSDeleteMemoriesResult(), Error);
					return;
				}

				FEverMemOSDeleteMemoriesResult Out;
				if (Result.IsValid())
				{
					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetNumberField(TEXT("Count"), Out.Count);

						const TArray<TSharedPtr<FJsonValue>>* FiltersArray = nullptr;
						if ((*ResultObj)->TryGetArrayField(TEXT("Filters"), FiltersArray) && FiltersArray)
						{
							for (const TSharedPtr<FJsonValue>& Val : *FiltersArray)
							{
								FString S;
								if (Val.IsValid() && Val->TryGetString(S))
								{
									Out.Filters.Add(S);
								}
							}
						}
					}
				}

				OnComplete.ExecuteIfBound(Out, FEverMemOSError::NoError());
			}));
}

// --- Conversation Meta ---

void UEverMemOSSubsystem::GetConversationMetadata(const FString& GroupId, FOnConvoMetaComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("group_id"), GroupId);

	HttpClient->Get(TEXT("/memories/conversation-meta"), QueryParams, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSConvoMetaData(), Error);
					return;
				}

				FEverMemOSConvoMetaData Meta;

				if (Result.IsValid())
				{
					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetStringField(TEXT("Id"), Meta.Id);
						(*ResultObj)->TryGetStringField(TEXT("GroupId"), Meta.GroupId);
						(*ResultObj)->TryGetStringField(TEXT("Name"), Meta.Name);
						(*ResultObj)->TryGetStringField(TEXT("ConversationCreatedAt"), Meta.ConversationCreatedAt);
						(*ResultObj)->TryGetStringField(TEXT("CreatedAt"), Meta.CreatedAt);
						(*ResultObj)->TryGetStringField(TEXT("UpdatedAt"), Meta.UpdatedAt);
					}
				}

				OnComplete.ExecuteIfBound(Meta, FEverMemOSError::NoError());
			}));
}

void UEverMemOSSubsystem::SetConversationMetadata(const FEverMemOSConvoMetaUpsertRequest& Request, FOnConvoMetaComplete OnComplete)
{
	if (bIsShuttingDown) return;

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("GroupId"), Request.GroupId);
	Body->SetStringField(TEXT("Name"), Request.Name);
	Body->SetStringField(TEXT("CreatedAt"), Request.CreatedAt);

	HttpClient->Post(TEXT("/memories/conversation-meta"), Body, EEverMemOSResponseMode::BaseAPI,
		FOnHttpJsonResponse::CreateLambda(
			[this, OnComplete](TSharedPtr<FJsonObject> Result, const FEverMemOSError& Error)
			{
				if (bIsShuttingDown) return;

				if (Error.IsError())
				{
					OnComplete.ExecuteIfBound(FEverMemOSConvoMetaData(), Error);
					return;
				}

				FEverMemOSConvoMetaData Meta;

				if (Result.IsValid())
				{
					const TSharedPtr<FJsonObject>* ResultObj = nullptr;
					if (Result->TryGetObjectField(TEXT("Result"), ResultObj) && ResultObj && ResultObj->IsValid())
					{
						(*ResultObj)->TryGetStringField(TEXT("Id"), Meta.Id);
						(*ResultObj)->TryGetStringField(TEXT("GroupId"), Meta.GroupId);
						(*ResultObj)->TryGetStringField(TEXT("Name"), Meta.Name);
						(*ResultObj)->TryGetStringField(TEXT("ConversationCreatedAt"), Meta.ConversationCreatedAt);
						(*ResultObj)->TryGetStringField(TEXT("CreatedAt"), Meta.CreatedAt);
						(*ResultObj)->TryGetStringField(TEXT("UpdatedAt"), Meta.UpdatedAt);
					}
				}

				OnComplete.ExecuteIfBound(Meta, FEverMemOSError::NoError());
			}));
}

void UEverMemOSSubsystem::CancelAllRequests()
{
	if (HttpClient.IsValid())
	{
		HttpClient->CancelAllRequests();
	}
}

void UEverMemOSSubsystem::SetDeploymentProfile(EEverMemOSDeploymentProfile Profile)
{
	UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>();
	if (Settings)
	{
		Settings->DeploymentProfile = Profile;
		Settings->ApplyProfileDefaults();
	}
}
