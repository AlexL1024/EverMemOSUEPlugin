// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EverMemOSSettings.h"
#include "EverMemOSErrors.h"
#include "Models/EverMemOSHealthTypes.h"
#include "Models/EverMemOSAddMemoryTypes.h"
#include "Models/EverMemOSGetMemoriesTypes.h"
#include "Models/EverMemOSMemorizeTypes.h"
#include "Models/EverMemOSSearchTypes.h"
#include "Models/EverMemOSDeleteMemoriesTypes.h"
#include "Models/EverMemOSConvoMetaTypes.h"
#include "EverMemOSSubsystem.generated.h"

class FEverMemOSHttpClient;
class IEverMemOSAuthProvider;

// C++ callback typedefs
DECLARE_DELEGATE_TwoParams(FOnHealthCheckComplete, const FEverMemOSHealthResponse&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnAddMemoryComplete, const FEverMemOSAddMemoryResponse&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnGetMemoriesComplete, const FEverMemOSGetMemoriesResult&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnMemorizeComplete, const FEverMemOSMemorizeResult&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnSearchComplete, const FEverMemOSSearchResult&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnDeleteMemoriesComplete, const FEverMemOSDeleteMemoriesResult&, const FEverMemOSError&);
DECLARE_DELEGATE_TwoParams(FOnConvoMetaComplete, const FEverMemOSConvoMetaData&, const FEverMemOSError&);

UCLASS()
class EVERMEMOSRUNTIME_API UEverMemOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	/** Rebuild the HTTP client (called when settings change) */
	void RebuildClient();

	// --- C++ API ---

	/** Set Bearer token for this game instance (runtime override, not saved to config). */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetRuntimeBearerToken(const FString& Token);

	/** Set HMAC secret for this game instance (runtime override, not saved to config). */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetRuntimeHMACSecret(const FString& Secret);

	/** Health check */
	void HealthCheck(FOnHealthCheckComplete OnComplete);

	/** Add a memory (Add Memories) */
	void AddMemory(const FEverMemOSAddMemoryRequest& Request, FOnAddMemoryComplete OnComplete);

	/** Get memories */
	void GetMemories(const FEverMemOSGetMemoriesParams& Params, FOnGetMemoriesComplete OnComplete);

	/** Memorize messages */
	void Memorize(const FEverMemOSMemorizeRequest& Request, FOnMemorizeComplete OnComplete);

	/** Search memories */
	void Search(const FEverMemOSSearchParams& Params, FOnSearchComplete OnComplete);

	/** Delete memories */
	void DeleteMemories(const FEverMemOSDeleteMemoriesRequest& Request, FOnDeleteMemoriesComplete OnComplete);

	/** Get conversation metadata */
	void GetConversationMetadata(const FString& GroupId, FOnConvoMetaComplete OnComplete);

	/** Create/update conversation metadata */
	void SetConversationMetadata(const FEverMemOSConvoMetaUpsertRequest& Request, FOnConvoMetaComplete OnComplete);

	// --- Blueprint API ---

	UFUNCTION(BlueprintCallable, Category = "EverMemOS|System",
		meta = (DeprecatedFunction, DeprecationMessage = "Use the EverMemOS HealthCheck async Blueprint node instead."))
	void K2_HealthCheck();

	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (DeprecatedFunction, DeprecationMessage = "Use the EverMemOS Memorize async Blueprint node instead."))
	void K2_Memorize(const FEverMemOSMemorizeRequest& Request);

	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (DeprecatedFunction, DeprecationMessage = "Use the EverMemOS SearchMemories async Blueprint node instead."))
	void K2_Search(const FEverMemOSSearchParams& Params);

	/** Switch deployment profile at runtime (rebuilds the HTTP client) */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Connection")
	void SetDeploymentProfile(EEverMemOSDeploymentProfile Profile);

	/** Cancel all active requests */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|System")
	void CancelAllRequests();

	/** Whether the subsystem is shutting down */
	bool IsShuttingDown() const { return bIsShuttingDown; }

private:
	void OnSettingsChanged();
	TSharedPtr<IEverMemOSAuthProvider> CreateAuthProvider() const;

	TSharedPtr<FEverMemOSHttpClient> HttpClient;
	bool bIsShuttingDown = false;
	FDelegateHandle SettingsChangedHandle;

	// Per-game-instance overrides so Blueprints don't need to write into Project Settings.
	bool bHasRuntimeBearerToken = false;
	bool bHasRuntimeHMACSecret = false;
	FString RuntimeBearerToken;
	FString RuntimeHMACSecret;
};
