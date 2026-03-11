// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EverMemOSSettings.generated.h"

UENUM(BlueprintType)
enum class EEverMemOSDeploymentProfile : uint8
{
	Cloud = 0 UMETA(DisplayName = "Cloud (api.evermind.ai)"),
	Local = 1 UMETA(DisplayName = "Local (localhost:1995)")
};

UENUM(BlueprintType)
enum class EEverMemOSAuthMethod : uint8
{
	Bearer = 0 UMETA(DisplayName = "Bearer Token"),
	HMAC = 1 UMETA(DisplayName = "HMAC-SHA256")
};

UCLASS(Config = EverMemOS, DefaultConfig, meta = (DisplayName = "EverMemOS"))
class EVERMEMOSRUNTIME_API UEverMemOSSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UEverMemOSSettings();

	/** Deployment profile: Cloud connects to api.evermind.ai, Local connects to localhost:1995 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection")
	EEverMemOSDeploymentProfile DeploymentProfile;

	/** Base URL for the API (auto-set when changing DeploymentProfile) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection")
	FString BaseURL;

	/** API version prefix (e.g. "v0") */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection")
	FString ApiVersion;

	/** Authentication method (only used in Cloud mode) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Authentication",
		meta = (EditCondition = "DeploymentProfile == EEverMemOSDeploymentProfile::Cloud"))
	EEverMemOSAuthMethod AuthMethod;

	/** Request timeout in seconds */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection", meta = (ClampMin = "1.0", ClampMax = "300.0"))
	float TimeoutSeconds;

	/** Maximum number of automatic retries for 429/5xx errors */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection", meta = (ClampMin = "0", ClampMax = "10"))
	int32 MaxRetries;

	/** Base delay between retries in seconds (exponential backoff) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection", meta = (ClampMin = "0.1", ClampMax = "30.0"))
	float RetryDelaySeconds;

	/**
	 * Bearer Token (TRANSIENT - not saved to config files).
	 * Set at runtime via SetBearerToken() or temporarily in the editor.
	 */
	UPROPERTY(Transient, EditAnywhere, Category = "EverMemOS|Authentication",
		meta = (PasswordField = "true", EditCondition = "DeploymentProfile == EEverMemOSDeploymentProfile::Cloud"))
	FString BearerToken;

	/**
	 * HMAC Secret Key (TRANSIENT - not saved to config files).
	 * Set at runtime via SetHMACSecret() or temporarily in the editor.
	 */
	UPROPERTY(Transient, EditAnywhere, Category = "EverMemOS|Authentication",
		meta = (PasswordField = "true", EditCondition = "DeploymentProfile == EEverMemOSDeploymentProfile::Cloud"))
	FString HMACSecretKey;

	/** Set Bearer Token at runtime */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetBearerToken(const FString& InToken);

	/** Set HMAC Secret Key at runtime */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetHMACSecret(const FString& InSecret);

	/** Get the default base URL for a deployment profile */
	static FString GetDefaultBaseURL(EEverMemOSDeploymentProfile Profile);

	/** Get the default API version for a deployment profile */
	static FString GetDefaultApiVersion(EEverMemOSDeploymentProfile Profile);

	/** Get the status path segment ("status" for Cloud, "stats" for Local) */
	static FString GetStatusPathSegment(EEverMemOSDeploymentProfile Profile);

	/** Whether authentication is required for a deployment profile */
	static bool RequiresAuth(EEverMemOSDeploymentProfile Profile);

	/** Apply profile defaults to BaseURL and ApiVersion, then broadcast change */
	void ApplyProfileDefaults();

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
	virtual FName GetSectionName() const override { return FName(TEXT("EverMemOS")); }

#if WITH_EDITOR
	virtual FText GetSectionText() const override { return NSLOCTEXT("EverMemOS", "SettingsSection", "EverMemOS"); }
	virtual FText GetSectionDescription() const override { return NSLOCTEXT("EverMemOS", "SettingsDesc", "Configure the EverMemOS plugin"); }
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	DECLARE_MULTICAST_DELEGATE(FOnSettingsChanged);
	FOnSettingsChanged OnSettingsChanged;
};
