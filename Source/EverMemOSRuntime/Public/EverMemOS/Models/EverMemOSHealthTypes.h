// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSHealthTypes.generated.h"

/** Health check response */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSHealthResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Status;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Message;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Service;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Version;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Timestamp;
};
