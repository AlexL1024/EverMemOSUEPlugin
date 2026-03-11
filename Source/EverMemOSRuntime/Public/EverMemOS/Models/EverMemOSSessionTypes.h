// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSSessionTypes.generated.h"

// Phase 3: Session endpoint types (stub)

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSDeleteSessionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	bool bSuccess = false;
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSRedactTurnResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	bool bSuccess = false;
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSSessionExport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString SessionId;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString ExportData;
};
