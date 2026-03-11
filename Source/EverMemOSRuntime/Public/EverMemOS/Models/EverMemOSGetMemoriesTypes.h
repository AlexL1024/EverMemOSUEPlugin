// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSEnums.h"
#include "EverMemOSGetMemoriesTypes.generated.h"

/** Memory record returned by GET /api/v0/memories and search */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSMemoryRecord
{
	GENERATED_BODY()

	/** Optional internal id if returned */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString UserId;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString GroupId;

	/** Memory type string (e.g. episodic_memory) */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString MemoryType;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Summary;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Content;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Timestamp;
};

/** Query params for GET /api/v0/memories */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSGetMemoriesParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString UserId;

	/** Optional group_ids list */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TArray<FString> GroupIds;

	/** Optional memory_type filter (string form like "episodic_memory"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString MemoryType;

	/** Optional time window (start_time/end_time) in ISO 8601 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString EndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS", meta = (ClampMin = "1", ClampMax = "100"))
	int32 PageSize = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS", meta = (ClampMin = "1", ClampMax = "100000"))
	int32 Page = 1;
};

/** Result payload for GET /api/v0/memories */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSGetMemoriesResult
{
	GENERATED_BODY()

	/** Count returned by the service (count) */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	int32 Count = 0;

	/** Total count (total_count) if returned */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	int32 TotalCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TArray<FEverMemOSMemoryRecord> Memories;

	/** Optional metadata returned by the service (best-effort string map). */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TMap<FString, FString> Metadata;
};
