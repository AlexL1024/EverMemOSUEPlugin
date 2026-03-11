// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSConvoMetaSetAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSConvoMetaSetAction* UEverMemOSConvoMetaSetAction::SetConversationMetadata(UObject* WorldContextObject, const FEverMemOSConvoMetaUpsertRequest& Request)
{
	UEverMemOSConvoMetaSetAction* Action = NewObject<UEverMemOSConvoMetaSetAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->UpsertRequest = Request;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSConvoMetaSetAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->SetConversationMetadata(UpsertRequest, FOnConvoMetaComplete::CreateLambda(
		[this](const FEverMemOSConvoMetaData& Result, const FEverMemOSError& Error)
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

