// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EverMemOSSettings.generated.h"

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

	/** Base URL for the Evermind Cloud API */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection")
	FString BaseURL;

	/** API version prefix (e.g. "v0") */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Connection")
	FString ApiVersion;

	/** Authentication method */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EverMemOS|Authentication")
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
		UPROPERTY(Transient, EditAnywhere, Category = "EverMemOS|Authentication", meta = (PasswordField = "true"))
		FString BearerToken;

		/**
		 * HMAC Secret Key (TRANSIENT - not saved to config files).
		 * Set at runtime via SetHMACSecret() or temporarily in the editor.
		 */
		UPROPERTY(Transient, EditAnywhere, Category = "EverMemOS|Authentication", meta = (PasswordField = "true"))
		FString HMACSecretKey;

	/** Set Bearer Token at runtime */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetBearerToken(const FString& InToken);

	/** Set HMAC Secret Key at runtime */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Authentication")
	void SetHMACSecret(const FString& InSecret);

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
	virtual FName GetSectionName() const override { return FName(TEXT("EverMemOS")); }

#if WITH_EDITOR
	virtual FText GetSectionText() const override { return NSLOCTEXT("EverMemOS", "SettingsSection", "EverMemOS"); }
	virtual FText GetSectionDescription() const override { return NSLOCTEXT("EverMemOS", "SettingsDesc", "Configure the EverMemOS plugin for Evermind Cloud API"); }
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	DECLARE_MULTICAST_DELEGATE(FOnSettingsChanged);
	FOnSettingsChanged OnSettingsChanged;
};
