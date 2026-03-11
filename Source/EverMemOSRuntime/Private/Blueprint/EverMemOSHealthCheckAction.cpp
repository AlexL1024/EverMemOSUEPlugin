// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSHealthCheckAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSHealthCheckAction* UEverMemOSHealthCheckAction::HealthCheck(UObject* WorldContextObject)
{
	UEverMemOSHealthCheckAction* Action = NewObject<UEverMemOSHealthCheckAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSHealthCheckAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->HealthCheck(FOnHealthCheckComplete::CreateLambda(
		[this](const FEverMemOSHealthResponse& Response, const FEverMemOSError& Error)
		{
			if (Error.IsError())
			{
				BroadcastError(Error);
			}
			else
			{
				OnSuccess.Broadcast(Response);
				SetReadyToDestroy();
			}
		}));
}
