// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSAddMemoryAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSAddMemoryAction* UEverMemOSAddMemoryAction::AddMemory(UObject* WorldContextObject,
	const FEverMemOSAddMemoryRequest& Request)
{
	UEverMemOSAddMemoryAction* Action = NewObject<UEverMemOSAddMemoryAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->AddRequest = Request;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSAddMemoryAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->AddMemory(AddRequest, FOnAddMemoryComplete::CreateLambda(
		[this](const FEverMemOSAddMemoryResponse& Result, const FEverMemOSError& Error)
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

