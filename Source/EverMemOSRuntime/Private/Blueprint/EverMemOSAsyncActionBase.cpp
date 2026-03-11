// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSAsyncActionBase.h"
#include "EverMemOS/EverMemOSSubsystem.h"
#include "EverMemOS/EverMemOSLog.h"
#include "Engine/GameInstance.h"

UEverMemOSSubsystem* UEverMemOSAsyncActionBase::GetSubsystem(const UObject* WorldContextObject) const
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UEverMemOSSubsystem>();
}

void UEverMemOSAsyncActionBase::BroadcastError(const FEverMemOSError& Error)
{
	const UEnum* Enum = StaticEnum<EEverMemOSErrorType>();
	const FString TypeString = Enum ? Enum->GetNameStringByValue(static_cast<int64>(Error.ErrorType)) : TEXT("Unknown");
	UE_LOG(LogEverMemOS, Warning, TEXT("EverMemOS Blueprint call failed: Type=%s Status=%d Message=%s"),
		*TypeString, Error.StatusCode, *Error.Message);

	OnFailure.Broadcast(Error);
	SetReadyToDestroy();
}
