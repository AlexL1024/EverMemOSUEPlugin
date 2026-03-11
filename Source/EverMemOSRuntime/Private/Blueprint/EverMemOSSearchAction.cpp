// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSSearchAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSSearchAction* UEverMemOSSearchAction::SearchMemories(UObject* WorldContextObject,
	const FEverMemOSSearchParams& Params)
{
	UEverMemOSSearchAction* Action = NewObject<UEverMemOSSearchAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->SearchParams = Params;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSSearchAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->Search(SearchParams, FOnSearchComplete::CreateLambda(
		[this](const FEverMemOSSearchResult& Result, const FEverMemOSError& Error)
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
