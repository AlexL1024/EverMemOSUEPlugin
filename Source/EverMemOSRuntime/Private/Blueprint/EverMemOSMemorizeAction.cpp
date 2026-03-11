// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSMemorizeAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSMemorizeAction* UEverMemOSMemorizeAction::Memorize(UObject* WorldContextObject,
	const FEverMemOSMemorizeRequest& Request)
{
	UEverMemOSMemorizeAction* Action = NewObject<UEverMemOSMemorizeAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->MemorizeRequest = Request;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSMemorizeAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->Memorize(MemorizeRequest, FOnMemorizeComplete::CreateLambda(
		[this](const FEverMemOSMemorizeResult& Result, const FEverMemOSError& Error)
		{
			if (Error.IsError())
			{
				BroadcastError(Error);
			}
			else
			{
				OnSuccess.Broadcast(Result);
				SetReadyToDestroy();
			}
		}));
}
