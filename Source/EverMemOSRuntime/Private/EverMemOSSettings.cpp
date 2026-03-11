// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/EverMemOSSettings.h"
#include "EverMemOS/EverMemOSLog.h"

UEverMemOSSettings::UEverMemOSSettings()
	: BaseURL(TEXT("https://api.evermind.ai"))
	, ApiVersion(TEXT("v0"))
	, AuthMethod(EEverMemOSAuthMethod::Bearer)
	, TimeoutSeconds(30.0f)
	, MaxRetries(3)
	, RetryDelaySeconds(1.0f)
{
}

void UEverMemOSSettings::SetBearerToken(const FString& InToken)
{
	BearerToken = InToken;
	OnSettingsChanged.Broadcast();
}

void UEverMemOSSettings::SetHMACSecret(const FString& InSecret)
{
	HMACSecretKey = InSecret;
	OnSettingsChanged.Broadcast();
}

#if WITH_EDITOR
void UEverMemOSSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnSettingsChanged.Broadcast();
}
#endif
