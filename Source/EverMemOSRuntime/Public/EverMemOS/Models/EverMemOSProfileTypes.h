// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSProfileTypes.generated.h"

// Phase 3: Profile endpoint types (stub)

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSUpsertProfileRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TMap<FString, FString> ProfileData;
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSCustomProfileData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString UserId;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TMap<FString, FString> ProfileData;
};
