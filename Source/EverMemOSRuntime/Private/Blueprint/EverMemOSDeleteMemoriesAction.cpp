// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSDeleteMemoriesAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSDeleteMemoriesAction* UEverMemOSDeleteMemoriesAction::DeleteMemories(UObject* WorldContextObject,
	const FEverMemOSDeleteMemoriesRequest& Request)
{
	UEverMemOSDeleteMemoriesAction* Action = NewObject<UEverMemOSDeleteMemoriesAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->DeleteRequest = Request;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSDeleteMemoriesAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->DeleteMemories(DeleteRequest, FOnDeleteMemoriesComplete::CreateLambda(
		[this](const FEverMemOSDeleteMemoriesResult& Result, const FEverMemOSError& Error)
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

