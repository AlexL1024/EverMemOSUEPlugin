// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSConvoMetaTypes.generated.h"

// Conversation metadata types

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSConvoMetaUpsertRequest
{
	GENERATED_BODY()

	/** Conversation/group id used by the API (query/body: group_id) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString GroupId;

	/** Human-readable name (body: name) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString Name;

	/** Conversation created time (body: created_at) in ISO 8601 format */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EverMemOS")
	FString CreatedAt;
};

USTRUCT(BlueprintType)
struct EVERMEMOSRUNTIME_API FEverMemOSConvoMetaData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString GroupId;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString ConversationCreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "EverMemOS")
	FString UpdatedAt;
};
