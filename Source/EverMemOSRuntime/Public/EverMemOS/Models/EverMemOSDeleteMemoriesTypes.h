// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSDeleteMemoriesTypes.generated.h"

/** Request body for DELETE /api/v0/memories */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSDeleteMemoriesRequest
{
	GENERATED_BODY()

	/** Delete by memory_id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString MemoryId;

	/** Delete by id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Id;

	/** Delete by event_id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString EventId;

	/** Filter by user_id (defaults to "__all__" if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString UserId;

	/** Filter by group_id (defaults to "__all__" if empty) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString GroupId;
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSDeleteMemoriesResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	int32 Count = 0;

	/** Filters the service applied (if present). */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TArray<FString> Filters;
};
