// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSBlueprintLibrary.h"
#include "EverMemOS/EverMemOSSettings.h"
#include "EverMemOS/EverMemOSSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

void UEverMemOSBlueprintLibrary::SetBearerToken(const FString& Token)
{
	if (UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>())
	{
		Settings->SetBearerToken(Token);
	}
}

void UEverMemOSBlueprintLibrary::SetHMACSecret(const FString& Secret)
{
	if (UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>())
	{
		Settings->SetHMACSecret(Secret);
	}
}

void UEverMemOSBlueprintLibrary::SetBearerTokenRuntime(UObject* WorldContextObject, const FString& Token)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World)
	{
		return;
	}

	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UEverMemOSSubsystem* Subsystem = GameInstance->GetSubsystem<UEverMemOSSubsystem>())
		{
			Subsystem->SetRuntimeBearerToken(Token);
		}
	}
}

void UEverMemOSBlueprintLibrary::SetHMACSecretRuntime(UObject* WorldContextObject, const FString& Secret)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World)
	{
		return;
	}

	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UEverMemOSSubsystem* Subsystem = GameInstance->GetSubsystem<UEverMemOSSubsystem>())
		{
			Subsystem->SetRuntimeHMACSecret(Secret);
		}
	}
}

FString UEverMemOSBlueprintLibrary::ErrorToString(const FEverMemOSError& Error)
{
	const UEnum* Enum = StaticEnum<EEverMemOSErrorType>();
	const FString TypeString = Enum ? Enum->GetNameStringByValue(static_cast<int64>(Error.ErrorType)) : TEXT("Unknown");
	return FString::Printf(TEXT("EverMemOS Error: Type=%s Status=%d Message=%s"),
		*TypeString,
		Error.StatusCode,
		*Error.Message);
}

FEverMemOSConvoMetaUpsertRequest UEverMemOSBlueprintLibrary::MakeConvoMetaRequest(const FString& GroupId,
	const FString& Name, const FString& CreatedAt)
{
	FEverMemOSConvoMetaUpsertRequest Request;
	Request.GroupId = GroupId;
	Request.Name = Name;
	Request.CreatedAt = CreatedAt;
	return Request;
}

FString UEverMemOSBlueprintLibrary::UtcNowIso8601()
{
	return FDateTime::UtcNow().ToIso8601();
}

FEverMemOSMessage UEverMemOSBlueprintLibrary::MakeMessage(EEverMemOSMessageRole Role, const FString& Content)
{
	FEverMemOSMessage Msg;
	Msg.Role = Role;
	Msg.Content = Content;
	return Msg;
}

FEverMemOSMemorizeRequest UEverMemOSBlueprintLibrary::MakeMemorizeRequest(const FString& UserId,
	const TArray<FEverMemOSMessage>& Messages, const FString& SessionId)
{
	FEverMemOSMemorizeRequest Request;
	Request.UserId = UserId;
	Request.Messages = Messages;
	Request.SessionId = SessionId;
	return Request;
}

FEverMemOSSearchParams UEverMemOSBlueprintLibrary::MakeSearchParams(const FString& UserId,
	const FString& Query, EEverMemOSRetrieveMethod Method, int32 TopK)
{
	FEverMemOSSearchParams Params;
	Params.UserId = UserId;
	Params.Query = Query;
	Params.RetrieveMethod = Method;
	Params.TopK = TopK;
	return Params;
}

FEverMemOSAddMemoryRequest UEverMemOSBlueprintLibrary::MakeAddMemoryRequest(const FString& MessageId,
	const FString& Content, const FString& Sender,
	const FString& CreateTimeIso8601,
	const FString& GroupId)
{
	FEverMemOSAddMemoryRequest Request;
	Request.MessageId = MessageId;
	Request.Content = Content;
	Request.Sender = Sender;
	Request.CreateTime = CreateTimeIso8601;
	Request.GroupId = GroupId;
	return Request;
}

FEverMemOSGetMemoriesParams UEverMemOSBlueprintLibrary::MakeGetMemoriesParams(const FString& UserId,
	const TArray<FString>& GroupIds,
	int32 Page, int32 PageSize)
{
	FEverMemOSGetMemoriesParams Params;
	Params.UserId = UserId;
	Params.GroupIds = GroupIds;
	Params.Page = Page;
	Params.PageSize = PageSize;
	return Params;
}

FEverMemOSDeleteMemoriesRequest UEverMemOSBlueprintLibrary::MakeDeleteMemoriesRequest(const FString& UserId,
	const FString& GroupId,
	const FString& MemoryId)
{
	FEverMemOSDeleteMemoriesRequest Request;
	Request.UserId = UserId;
	Request.GroupId = GroupId;
	Request.MemoryId = MemoryId;
	return Request;
}

FString UEverMemOSBlueprintLibrary::MemoryTypeToString(EEverMemOSMemoryType Type)
{
	switch (Type)
	{
	case EEverMemOSMemoryType::Profile: return TEXT("profile");
	case EEverMemOSMemoryType::EpisodicMemory: return TEXT("episodic_memory");
	case EEverMemOSMemoryType::Foresight: return TEXT("foresight");
	case EEverMemOSMemoryType::EventLog: return TEXT("event_log");
	case EEverMemOSMemoryType::GroupProfile: return TEXT("group_profile");
	default: return TEXT("episodic_memory");
	}
}

FString UEverMemOSBlueprintLibrary::RetrieveMethodToString(EEverMemOSRetrieveMethod Method)
{
	switch (Method)
	{
	case EEverMemOSRetrieveMethod::Keyword: return TEXT("keyword");
	case EEverMemOSRetrieveMethod::Vector: return TEXT("vector");
	case EEverMemOSRetrieveMethod::Hybrid: return TEXT("hybrid");
	case EEverMemOSRetrieveMethod::RRF: return TEXT("rrf");
	case EEverMemOSRetrieveMethod::Agentic: return TEXT("agentic");
	default: return TEXT("hybrid");
	}
}

FString UEverMemOSBlueprintLibrary::MessageRoleToString(EEverMemOSMessageRole Role)
{
	switch (Role)
	{
	case EEverMemOSMessageRole::User: return TEXT("user");
	case EEverMemOSMessageRole::Assistant: return TEXT("assistant");
	default: return TEXT("user");
	}
}

FString UEverMemOSBlueprintLibrary::SceneTypeToString(EEverMemOSSceneType Type)
{
	switch (Type)
	{
	case EEverMemOSSceneType::GroupChat: return TEXT("group_chat");
	case EEverMemOSSceneType::Assistant: return TEXT("assistant");
	default: return TEXT("assistant");
	}
}
