// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSGetMemoriesAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSGetMemoriesAction* UEverMemOSGetMemoriesAction::GetMemories(UObject* WorldContextObject,
	const FEverMemOSGetMemoriesParams& Params)
{
	UEverMemOSGetMemoriesAction* Action = NewObject<UEverMemOSGetMemoriesAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->GetParams = Params;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSGetMemoriesAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->GetMemories(GetParams, FOnGetMemoriesComplete::CreateLambda(
		[this](const FEverMemOSGetMemoriesResult& Result, const FEverMemOSError& Error)
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

