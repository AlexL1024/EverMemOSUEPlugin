// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "EverMemOS/EverMemOSErrors.h"
#include "EverMemOSRetryPolicy.h"

class IEverMemOSAuthProvider;
class UWorld;

/** Response decode modes matching the Swift SDK */
enum class EEverMemOSResponseMode : uint8
{
	/** { status, message, result: T } */
	BaseAPI,
	/** { success, found?, data?: T } */
	Success,
	/** Direct JSON object */
	Bare
};

DECLARE_DELEGATE_TwoParams(FOnHttpJsonResponse, TSharedPtr<FJsonObject> /* Result */, const FEverMemOSError& /* Error */);

/**
 * HTTP transport layer for EverMemOS API.
 * Handles request creation, auth injection, response decoding, and automatic retries.
 */
class EVERMEMOSRUNTIME_API FEverMemOSHttpClient : public TSharedFromThis<FEverMemOSHttpClient>
{
public:
	FEverMemOSHttpClient();
	~FEverMemOSHttpClient();

	void Configure(const FString& InBaseURL, const FString& InApiVersion,
		const FString& InStatusPathSegment,
		float InTimeout, TSharedPtr<IEverMemOSAuthProvider> InAuthProvider,
		const FEverMemOSRetryPolicy& InRetryPolicy);

	/** Get the status path segment ("status" for Cloud, "stats" for Local) */
	const FString& GetStatusPathSegment() const { return StatusPathSegment; }

	/** Send a GET request */
	FHttpRequestRef Get(const FString& Endpoint, const TMap<FString, FString>& QueryParams,
		EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);

	/** Send a GET request without the /api/{version} prefix (e.g., for /health). */
	FHttpRequestRef GetRaw(const FString& Path, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);

	/** Send a POST request with JSON body */
	FHttpRequestRef Post(const FString& Endpoint, TSharedPtr<FJsonObject> Body,
		EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);

	/** Send a DELETE request */
	FHttpRequestRef Delete(const FString& Endpoint, TSharedPtr<FJsonObject> Body,
		EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);

	/** Send a PATCH request with JSON body */
	FHttpRequestRef Patch(const FString& Endpoint, TSharedPtr<FJsonObject> Body,
		EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);

	/** Remove a request from the active tracking list */
	void RemoveActiveRequest(FHttpRequestPtr Request);

	/** Cancel a specific request */
	void CancelRequest(FHttpRequestRef Request);

	/** Cancel all active requests */
	void CancelAllRequests();

	/** Get all active requests */
	const TArray<FHttpRequestRef>& GetActiveRequests() const { return ActiveRequests; }

private:
	FHttpRequestRef CreateRequestInternal(const FString& Verb, const FString& URL, TSharedPtr<FJsonObject> Body);
	FHttpRequestRef SendWithBody(const FString& Verb, const FString& Endpoint,
		TSharedPtr<FJsonObject> Body, EEverMemOSResponseMode Mode, FOnHttpJsonResponse OnComplete);
	FString BuildURL(const FString& Endpoint, const TMap<FString, FString>& QueryParams = {}) const;
	FString BuildURLRaw(const FString& Path) const;
	void ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response,
		bool bConnectedSuccessfully, EEverMemOSResponseMode Mode,
		FOnHttpJsonResponse OnComplete, int32 AttemptNumber,
		const FString& Verb, const FString& Endpoint,
		TSharedPtr<FJsonObject> Body);
	void DecodeResponse(FHttpResponsePtr Response, EEverMemOSResponseMode Mode,
		TSharedPtr<FJsonObject>& OutResult, FEverMemOSError& OutError);

	void CancelRetryTimers();

	struct FRetryTimer
	{
		TWeakObjectPtr<UWorld> World;
		FTimerHandle Handle;
	};

	FString BaseURL;
	FString ApiVersion;
	FString StatusPathSegment;
	float Timeout;
	TSharedPtr<IEverMemOSAuthProvider> AuthProvider;
	FEverMemOSRetryPolicy RetryPolicy;
	TArray<FHttpRequestRef> ActiveRequests;
	TArray<FRetryTimer> ActiveRetryTimers;
};
