// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSErrors.generated.h"

UENUM(BlueprintType)
enum class EEverMemOSErrorType : uint8
{
	None = 0,
	Network,
	Timeout,
	Authentication,
	RateLimit,
	ServerError,
	InvalidResponse,
	Cancelled,
	Unknown
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSError
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	int32 StatusCode = 0;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Message;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	EEverMemOSErrorType ErrorType = EEverMemOSErrorType::None;

	bool IsError() const { return ErrorType != EEverMemOSErrorType::None; }

	static FEverMemOSError NoError() { return FEverMemOSError(); }

	static FEverMemOSError FromHttp(int32 InStatusCode, const FString& InMessage)
	{
		FEverMemOSError Error;
		Error.StatusCode = InStatusCode;
		Error.Message = InMessage;

		if (InStatusCode == 401 || InStatusCode == 403)
		{
			Error.ErrorType = EEverMemOSErrorType::Authentication;
		}
		else if (InStatusCode == 429)
		{
			Error.ErrorType = EEverMemOSErrorType::RateLimit;
		}
		else if (InStatusCode >= 500)
		{
			Error.ErrorType = EEverMemOSErrorType::ServerError;
		}
		else
		{
			Error.ErrorType = EEverMemOSErrorType::Unknown;
		}

		return Error;
	}

	static FEverMemOSError NetworkError(const FString& InMessage)
	{
		FEverMemOSError Error;
		Error.StatusCode = 0;
		Error.Message = InMessage;
		Error.ErrorType = EEverMemOSErrorType::Network;
		return Error;
	}

	static FEverMemOSError TimeoutError()
	{
		FEverMemOSError Error;
		Error.StatusCode = 0;
		Error.Message = TEXT("Request timed out");
		Error.ErrorType = EEverMemOSErrorType::Timeout;
		return Error;
	}

	static FEverMemOSError CancelledError()
	{
		FEverMemOSError Error;
		Error.StatusCode = 0;
		Error.Message = TEXT("Request was cancelled");
		Error.ErrorType = EEverMemOSErrorType::Cancelled;
		return Error;
	}
};
