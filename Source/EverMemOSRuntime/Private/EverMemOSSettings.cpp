// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/EverMemOSSettings.h"
#include "EverMemOS/EverMemOSLog.h"

UEverMemOSSettings::UEverMemOSSettings()
	: DeploymentProfile(EEverMemOSDeploymentProfile::Cloud)
	, BaseURL(TEXT("https://api.evermind.ai"))
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

FString UEverMemOSSettings::GetDefaultBaseURL(EEverMemOSDeploymentProfile Profile)
{
	switch (Profile)
	{
	case EEverMemOSDeploymentProfile::Local:
		return TEXT("http://localhost:1995");
	default:
		return TEXT("https://api.evermind.ai");
	}
}

FString UEverMemOSSettings::GetDefaultApiVersion(EEverMemOSDeploymentProfile Profile)
{
	switch (Profile)
	{
	case EEverMemOSDeploymentProfile::Local:
		return TEXT("v1");
	default:
		return TEXT("v0");
	}
}

FString UEverMemOSSettings::GetStatusPathSegment(EEverMemOSDeploymentProfile Profile)
{
	switch (Profile)
	{
	case EEverMemOSDeploymentProfile::Local:
		return TEXT("stats");
	default:
		return TEXT("status");
	}
}

bool UEverMemOSSettings::RequiresAuth(EEverMemOSDeploymentProfile Profile)
{
	return Profile == EEverMemOSDeploymentProfile::Cloud;
}

void UEverMemOSSettings::ApplyProfileDefaults()
{
	BaseURL = GetDefaultBaseURL(DeploymentProfile);
	ApiVersion = GetDefaultApiVersion(DeploymentProfile);
	OnSettingsChanged.Broadcast();
}

#if WITH_EDITOR
void UEverMemOSSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEverMemOSSettings, DeploymentProfile))
	{
		ApplyProfileDefaults();
		return;
	}

	OnSettingsChanged.Broadcast();
}
#endif
