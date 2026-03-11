// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSEnums.h"
#include "EverMemOSMemorizeTypes.generated.h"

/** A single message in a memorize request */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	EEverMemOSMessageRole Role = EEverMemOSMessageRole::User;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Content;
};

/** Request body for the Memorize endpoint */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSMemorizeRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TArray<FEverMemOSMessage> Messages;

	/** Optional session/conversation ID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString SessionId;

	/** Optional scene type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	EEverMemOSSceneType SceneType = EEverMemOSSceneType::Assistant;
};

/** Result from the Memorize endpoint */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSMemorizeResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Status;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Message;
};
