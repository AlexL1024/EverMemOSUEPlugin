// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSGetMemoriesTypes.h"
#include "EverMemOSEnums.h"
#include "EverMemOSSearchTypes.generated.h"

/** Search request parameters */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSSearchParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TArray<FString> GroupIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Query;

	/** Retrieval method (retrieve_method) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	EEverMemOSRetrieveMethod RetrieveMethod = EEverMemOSRetrieveMethod::Hybrid;

	/** Optional memory types filter (memory_types) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TArray<EEverMemOSMemoryType> MemoryTypes;

	/** Whether to include metadata in the response (include_metadata) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	bool bIncludeMetadata = false;

	/** Optional time window (start_time/end_time/current_time) in ISO 8601 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString EndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString CurrentTime;

	/** Optional radius for time-based search (radius) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS", meta = (ClampMin = "0", ClampMax = "3650"))
	int32 Radius = 0;

	/** Max number of results (top_k). -1 means default/server-side behavior. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS", meta = (ClampMin = "-1", ClampMax = "200"))
	int32 TopK = -1;
};

/** Search result */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSSearchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	int32 TotalCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TArray<FEverMemOSMemoryRecord> Memories;

	/** Optional metadata returned by the service (best-effort string map). */
	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	TMap<FString, FString> Metadata;
};
