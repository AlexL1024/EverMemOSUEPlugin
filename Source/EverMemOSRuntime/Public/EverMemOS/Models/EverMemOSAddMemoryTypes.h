// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAddMemoryTypes.generated.h"

/** Request body for POST /api/v0/memories (Add Memories) */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSAddMemoryRequest
{
	GENERATED_BODY()

	/** Unique message id (message_id) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString MessageId;

	/** Message create time in ISO 8601 format (create_time) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString CreateTime;

	/** Sender id (sender) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Sender;

	/** Message content (content) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Content;

	/** Optional group/conversation id (group_id) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString GroupId;

	/** Optional group name (group_name) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString GroupName;

	/** Optional sender name (sender_name) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString SenderName;

	/** Optional role string (role) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Role;

	/** Optional referenced memory ids (refer_list) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	TArray<FString> ReferList;

	/** Whether to flush immediately (flush) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	bool bFlush = false;
};

/** Response for Add Memories */
USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSAddMemoryResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Status;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Message;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString RequestId;
};

