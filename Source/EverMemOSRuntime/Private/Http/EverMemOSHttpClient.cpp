// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Http/EverMemOSHttpClient.h"
#include "EverMemOS/Auth/IEverMemOSAuthProvider.h"
#include "EverMemOS/EverMemOSLog.h"
#include "EverMemOSJsonHelper.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Async/Async.h"
#include "TimerManager.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

static UWorld* GetEverMemOSRetryWorld()
{
	if (!GEngine)
	{
		return nullptr;
	}

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (UWorld* World = Context.World())
		{
			return World;
		}
	}

	return nullptr;
}

FEverMemOSHttpClient::FEverMemOSHttpClient()
	: Timeout(30.0f)
{
}

FEverMemOSHttpClient::~FEverMemOSHttpClient()
{
	CancelAllRequests();
}

void FEverMemOSHttpClient::CancelRetryTimers()
{
	for (FRetryTimer& Timer : ActiveRetryTimers)
	{
		if (UWorld* World = Timer.World.Get())
		{
			World->GetTimerManager().ClearTimer(Timer.Handle);
		}
	}
	ActiveRetryTimers.Empty();
}

void FEverMemOSHttpClient::Configure(const FString& InBaseURL, const FString& InApiVersion,
	float InTimeout, TSharedPtr<IEverMemOSAuthProvider> InAuthProvider,
	const FEverMemOSRetryPolicy& InRetryPolicy)
{
	BaseURL = InBaseURL;
	ApiVersion = InApiVersion;
	Timeout = InTimeout;
	AuthProvider = InAuthProvider;
	RetryPolicy = InRetryPolicy;
}

FString FEverMemOSHttpClient::BuildURL(const FString& Endpoint, const TMap<FString, FString>& QueryParams) const
{
	const FString NormalizedBase = BaseURL.EndsWith(TEXT("/")) ? BaseURL.LeftChop(1) : BaseURL;

	FString NormalizedVersion = ApiVersion;
	while (NormalizedVersion.StartsWith(TEXT("/")))
	{
		NormalizedVersion.RightChopInline(1);
	}
	while (NormalizedVersion.EndsWith(TEXT("/")))
	{
		NormalizedVersion.LeftChopInline(1);
	}

	const FString NormalizedEndpoint = Endpoint.StartsWith(TEXT("/")) ? Endpoint : (TEXT("/") + Endpoint);

	FString URL = FString::Printf(TEXT("%s/api/%s%s"), *NormalizedBase, *NormalizedVersion, *NormalizedEndpoint);

	if (QueryParams.Num() > 0)
	{
		URL += TEXT("?");
		bool bFirst = true;
		for (const auto& Param : QueryParams)
		{
			if (!bFirst)
			{
				URL += TEXT("&");
			}
			URL += FString::Printf(TEXT("%s=%s"),
				*FGenericPlatformHttp::UrlEncode(Param.Key),
				*FGenericPlatformHttp::UrlEncode(Param.Value));
			bFirst = false;
		}
	}

	return URL;
}

FString FEverMemOSHttpClient::BuildURLRaw(const FString& Path) const
{
	const FString NormalizedBase = BaseURL.EndsWith(TEXT("/")) ? BaseURL.LeftChop(1) : BaseURL;
	const FString NormalizedPath = Path.StartsWith(TEXT("/")) ? Path : (TEXT("/") + Path);
	return NormalizedBase + NormalizedPath;
}

FHttpRequestRef FEverMemOSHttpClient::CreateRequest(const FString& Verb, const FString& Endpoint,
	TSharedPtr<FJsonObject> Body)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildURL(Endpoint));
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetTimeout(Timeout);

	if (Body.IsValid())
	{
		// Convert PascalCase keys to snake_case for API
		TSharedPtr<FJsonObject> SnakeBody = FEverMemOSJsonHelper::ConvertKeysToSnake(Body);

		FString BodyString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
		FJsonSerializer::Serialize(SnakeBody.ToSharedRef(), Writer);
		Request->SetContentAsString(BodyString);
	}

	if (AuthProvider.IsValid())
	{
		AuthProvider->ApplyAuth(Request);
	}

	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::CreateRawRequest(const FString& Verb, const FString& Path, TSharedPtr<FJsonObject> Body)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildURLRaw(Path));
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetTimeout(Timeout);

	if (Body.IsValid())
	{
		// Convert PascalCase keys to snake_case for API
		TSharedPtr<FJsonObject> SnakeBody = FEverMemOSJsonHelper::ConvertKeysToSnake(Body);

		FString BodyString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
		FJsonSerializer::Serialize(SnakeBody.ToSharedRef(), Writer);
		Request->SetContentAsString(BodyString);
	}

	if (AuthProvider.IsValid())
	{
		AuthProvider->ApplyAuth(Request);
	}

	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::Get(const FString& Endpoint,
	const TMap<FString, FString>& QueryParams,
	EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildURL(Endpoint, QueryParams));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetTimeout(Timeout);

	if (AuthProvider.IsValid())
	{
		AuthProvider->ApplyAuth(Request);
	}

	ActiveRequests.Add(Request);

	TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
	Request->OnProcessRequestComplete().BindLambda(
		[WeakSelf, Mode, OnComplete, Endpoint](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin())
			{
				Self->ProcessResponse(Req, Resp, bSuccess, Mode, OnComplete, 0, TEXT("GET"), Endpoint, nullptr);
			}
		});

	Request->ProcessRequest();
	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::GetRaw(const FString& Path, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(BuildURLRaw(Path));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetTimeout(Timeout);

	if (AuthProvider.IsValid())
	{
		AuthProvider->ApplyAuth(Request);
	}

	ActiveRequests.Add(Request);

	TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
	Request->OnProcessRequestComplete().BindLambda(
		[WeakSelf, Mode, OnComplete](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin())
			{
				// Remove from tracked requests
				for (int32 i = Self->ActiveRequests.Num() - 1; i >= 0; --i)
				{
					if (&Self->ActiveRequests[i].Get() == Req.Get())
					{
						Self->ActiveRequests.RemoveAt(i);
						break;
					}
				}

				if (!bSuccess || !Resp.IsValid())
				{
					FEverMemOSError Error = FEverMemOSError::NetworkError(TEXT("Connection failed"));
					AsyncTask(ENamedThreads::GameThread, [OnComplete, Error]()
					{
						OnComplete.ExecuteIfBound(nullptr, Error);
					});
					return;
				}

				TSharedPtr<FJsonObject> Result;
				FEverMemOSError Error;
				Self->DecodeResponse(Resp, Mode, Result, Error);

				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result, Error]()
				{
					OnComplete.ExecuteIfBound(Result, Error);
				});
			}
		});

	Request->ProcessRequest();
	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::Post(const FString& Endpoint,
	TSharedPtr<FJsonObject> Body, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete)
{
	FHttpRequestRef Request = CreateRequest(TEXT("POST"), Endpoint, Body);
	ActiveRequests.Add(Request);

	TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
	Request->OnProcessRequestComplete().BindLambda(
		[WeakSelf, Mode, OnComplete, Endpoint, Body](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin())
			{
				Self->ProcessResponse(Req, Resp, bSuccess, Mode, OnComplete, 0, TEXT("POST"), Endpoint, Body);
			}
		});

	Request->ProcessRequest();
	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::Delete(const FString& Endpoint,
	TSharedPtr<FJsonObject> Body, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete)
{
	FHttpRequestRef Request = CreateRequest(TEXT("DELETE"), Endpoint, Body);
	ActiveRequests.Add(Request);

	TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
	Request->OnProcessRequestComplete().BindLambda(
		[WeakSelf, Mode, OnComplete, Endpoint, Body](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin())
			{
				Self->ProcessResponse(Req, Resp, bSuccess, Mode, OnComplete, 0, TEXT("DELETE"), Endpoint, Body);
			}
		});

	Request->ProcessRequest();
	return Request;
}

FHttpRequestRef FEverMemOSHttpClient::Patch(const FString& Endpoint,
	TSharedPtr<FJsonObject> Body, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete)
{
	FHttpRequestRef Request = CreateRequest(TEXT("PATCH"), Endpoint, Body);
	ActiveRequests.Add(Request);

	TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
	Request->OnProcessRequestComplete().BindLambda(
		[WeakSelf, Mode, OnComplete, Endpoint, Body](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin())
			{
				Self->ProcessResponse(Req, Resp, bSuccess, Mode, OnComplete, 0, TEXT("PATCH"), Endpoint, Body);
			}
		});

	Request->ProcessRequest();
	return Request;
}

void FEverMemOSHttpClient::CancelRequest(FHttpRequestRef Request)
{
	Request->CancelRequest();
	ActiveRequests.Remove(Request);
}

void FEverMemOSHttpClient::CancelAllRequests()
{
	CancelRetryTimers();

	for (FHttpRequestRef& Request : ActiveRequests)
	{
		Request->CancelRequest();
	}
	ActiveRequests.Empty();
}

void FEverMemOSHttpClient::ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully, EEverMemOSResponseMode Mode,
	FOnHttpJsonResponse OnComplete, int32 AttemptNumber,
	const FString& Verb, const FString& Endpoint,
	TSharedPtr<FJsonObject> Body)
{
	// Remove from tracked requests
	for (int32 i = ActiveRequests.Num() - 1; i >= 0; --i)
	{
		if (&ActiveRequests[i].Get() == Request.Get())
		{
			ActiveRequests.RemoveAt(i);
			break;
		}
	}

	if (!bConnectedSuccessfully || !Response.IsValid())
	{
		FEverMemOSError Error = FEverMemOSError::NetworkError(TEXT("Connection failed"));
		AsyncTask(ENamedThreads::GameThread, [OnComplete, Error]()
		{
			OnComplete.ExecuteIfBound(nullptr, Error);
		});
		return;
	}

	const int32 StatusCode = Response->GetResponseCode();

	// Check if we should retry
	if (RetryPolicy.ShouldRetry(StatusCode, AttemptNumber))
	{
		const float Delay = RetryPolicy.GetRetryDelay(AttemptNumber);
		UE_LOG(LogEverMemOS, Warning, TEXT("Request to %s returned %d, retrying in %.1fs (attempt %d/%d)"),
			*Endpoint, StatusCode, Delay, AttemptNumber + 1, RetryPolicy.GetMaxRetries());

		// Schedule retry with timer
		TSharedRef<FTimerHandle> RetryTimerHandle = MakeShared<FTimerHandle>();
		int32 NextAttempt = AttemptNumber + 1;
		TWeakPtr<FEverMemOSHttpClient> WeakSelf = AsShared();
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([WeakSelf, Verb, Endpoint, Body, Mode, OnComplete, NextAttempt, RetryTimerHandle]()
		{
			TSharedPtr<FEverMemOSHttpClient> Self = WeakSelf.Pin();
			if (!Self.IsValid())
			{
				return;
			}

			const FTimerHandle HandleCopy = *RetryTimerHandle;
			Self->ActiveRetryTimers.RemoveAll([HandleCopy](const FRetryTimer& Item)
			{
				return Item.Handle == HandleCopy;
			});

			FHttpRequestRef NewRequest = Self->CreateRequest(Verb, Endpoint, Body);
			Self->ActiveRequests.Add(NewRequest);

			TWeakPtr<FEverMemOSHttpClient> WeakSelfInner = WeakSelf;
			NewRequest->OnProcessRequestComplete().BindLambda(
				[WeakSelfInner, Mode, OnComplete, NextAttempt, Verb, Endpoint, Body](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
				{
					if (TSharedPtr<FEverMemOSHttpClient> InnerSelf = WeakSelfInner.Pin())
					{
						InnerSelf->ProcessResponse(Req, Resp, bSuccess, Mode, OnComplete, NextAttempt, Verb, Endpoint, Body);
					}
				});

			NewRequest->ProcessRequest();
		});

		if (UWorld* World = GetEverMemOSRetryWorld())
		{
			World->GetTimerManager().SetTimer(*RetryTimerHandle, TimerDelegate, Delay, false);
			ActiveRetryTimers.Add({World, *RetryTimerHandle});
			return;
		}

		// Fallback: if no world available, just execute immediately
		UE_LOG(LogEverMemOS, Warning, TEXT("No world available for timer, retrying immediately"));
		TimerDelegate.Execute();
		return;
	}

	// Decode the response
	TSharedPtr<FJsonObject> Result;
	FEverMemOSError Error;
	DecodeResponse(Response, Mode, Result, Error);

	AsyncTask(ENamedThreads::GameThread, [OnComplete, Result, Error]()
	{
		OnComplete.ExecuteIfBound(Result, Error);
	});
}

void FEverMemOSHttpClient::DecodeResponse(FHttpResponsePtr Response, EEverMemOSResponseMode Mode,
	TSharedPtr<FJsonObject>& OutResult, FEverMemOSError& OutError)
{
	const int32 StatusCode = Response->GetResponseCode();
	const FString Content = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		if (StatusCode >= 200 && StatusCode < 300)
		{
			// Success but non-JSON response
			OutResult = MakeShareable(new FJsonObject());
			OutResult->SetStringField(TEXT("RawContent"), Content);
		}
		else
		{
			OutError = FEverMemOSError::FromHttp(StatusCode, Content);
		}
		return;
	}

	// Convert API snake_case keys to PascalCase
	JsonObject = FEverMemOSJsonHelper::ConvertKeysToPascal(JsonObject);

	if (StatusCode < 200 || StatusCode >= 300)
	{
		FString Message;
		if (JsonObject->TryGetStringField(TEXT("Message"), Message))
		{
			OutError = FEverMemOSError::FromHttp(StatusCode, Message);
		}
		else
		{
			OutError = FEverMemOSError::FromHttp(StatusCode, Content);
		}
		return;
	}

	switch (Mode)
	{
	case EEverMemOSResponseMode::BaseAPI:
		{
			// { status, message, result: T }
			// Pass the full envelope so callers can read top-level fields (Status, Message)
			// as well as the nested Result object.
			OutResult = JsonObject;
		}
		break;

	case EEverMemOSResponseMode::Success:
		{
			// { success, found?, data?: T }
			bool bSuccess = false;
			JsonObject->TryGetBoolField(TEXT("Success"), bSuccess);
			if (!bSuccess)
			{
				FString Message;
				JsonObject->TryGetStringField(TEXT("Message"), Message);
				OutError = FEverMemOSError::FromHttp(StatusCode, Message.IsEmpty() ? TEXT("Request was not successful") : Message);
				return;
			}

			const TSharedPtr<FJsonObject>* DataObj;
			if (JsonObject->TryGetObjectField(TEXT("Data"), DataObj))
			{
				OutResult = *DataObj;
			}
			else
			{
				OutResult = JsonObject;
			}
		}
		break;

	case EEverMemOSResponseMode::Bare:
		OutResult = JsonObject;
		break;
	}
}
