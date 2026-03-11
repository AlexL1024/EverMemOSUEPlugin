// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EverMemOS/Models/EverMemOSAddMemoryTypes.h"
#include "EverMemOS/Models/EverMemOSGetMemoriesTypes.h"
#include "EverMemOS/Models/EverMemOSDeleteMemoriesTypes.h"
#include "EverMemOS/Models/EverMemOSMemorizeTypes.h"
#include "EverMemOS/Models/EverMemOSSearchTypes.h"
#include "EverMemOS/Models/EverMemOSConvoMetaTypes.h"
#include "EverMemOS/EverMemOSErrors.h"
#include "EverMemOSBlueprintLibrary.generated.h"

/** Blueprint utility functions for EverMemOS */
UCLASS()
class EVERMEMOSRUNTIME_API UEverMemOSBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Set Bearer token on the settings object (TRANSIENT - not saved). Visible in Project Settings while the editor is running. */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	static void SetBearerToken(const FString& Token);

	/** Set HMAC secret on the settings object (TRANSIENT - not saved). Visible in Project Settings while the editor is running. */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	static void SetHMACSecret(const FString& Secret);

	/** Set Bearer token for the current game instance (does not touch Project Settings). */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication", meta = (WorldContext = "WorldContextObject"))
	static void SetBearerTokenRuntime(UObject* WorldContextObject, const FString& Token);

	/** Set HMAC secret for the current game instance (does not touch Project Settings). */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication", meta = (WorldContext = "WorldContextObject"))
	static void SetHMACSecretRuntime(UObject* WorldContextObject, const FString& Secret);

	/** Convert an error struct to a readable string for logs/UI. */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString ErrorToString(const FEverMemOSError& Error);

	/** Create a conversation metadata upsert request */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Conversation Meta Request"))
	static FEverMemOSConvoMetaUpsertRequest MakeConvoMetaRequest(const FString& GroupId,
		const FString& Name, const FString& CreatedAt);

	/** Current UTC time formatted as ISO 8601 (e.g., 2026-03-11T00:00:00Z). */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString UtcNowIso8601();

	/** Create a message struct */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Message"))
	static FEverMemOSMessage MakeMessage(EEverMemOSMessageRole Role, const FString& Content);

	/** Create a memorize request */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Memorize Request"))
	static FEverMemOSMemorizeRequest MakeMemorizeRequest(const FString& UserId,
		const TArray<FEverMemOSMessage>& Messages, const FString& SessionId = TEXT(""));

	/** Create search parameters */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Search Params"))
	static FEverMemOSSearchParams MakeSearchParams(const FString& UserId,
		const FString& Query, EEverMemOSRetrieveMethod Method = EEverMemOSRetrieveMethod::Hybrid,
		int32 TopK = -1);

	/** Create an Add Memories request (POST /memories). */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Add Memory Request"))
	static FEverMemOSAddMemoryRequest MakeAddMemoryRequest(const FString& MessageId,
		const FString& Content, const FString& Sender,
		const FString& CreateTimeIso8601,
		const FString& GroupId = TEXT(""));

	/** Create Get Memories params (GET /memories). */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Get Memories Params"))
	static FEverMemOSGetMemoriesParams MakeGetMemoriesParams(const FString& UserId,
		const TArray<FString>& GroupIds,
		int32 Page = 1, int32 PageSize = 20);

	/** Create Delete Memories request (DELETE /memories). */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities",
		meta = (DisplayName = "Make Delete Memories Request"))
	static FEverMemOSDeleteMemoriesRequest MakeDeleteMemoriesRequest(const FString& UserId,
		const FString& GroupId = TEXT("__all__"),
		const FString& MemoryId = TEXT(""));

	/** Convert memory type enum to API string */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString MemoryTypeToString(EEverMemOSMemoryType Type);

	/** Convert retrieve method enum to API string */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString RetrieveMethodToString(EEverMemOSRetrieveMethod Method);

	/** Convert message role enum to API string */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString MessageRoleToString(EEverMemOSMessageRole Role);

	/** Convert scene type enum to API string */
	UFUNCTION(BlueprintPure, Category = "EverMemOS|Utilities")
	static FString SceneTypeToString(EEverMemOSSceneType Type);
};
