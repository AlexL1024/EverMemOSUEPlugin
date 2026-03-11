// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Blueprint/EverMemOSConvoMetaGetAction.h"
#include "EverMemOS/EverMemOSSubsystem.h"

UEverMemOSConvoMetaGetAction* UEverMemOSConvoMetaGetAction::GetConversationMetadata(UObject* WorldContextObject, const FString& InGroupId)
{
	UEverMemOSConvoMetaGetAction* Action = NewObject<UEverMemOSConvoMetaGetAction>();
	Action->WorldContextObject = WorldContextObject;
	Action->GroupId = InGroupId;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}

void UEverMemOSConvoMetaGetAction::Activate()
{
	UEverMemOSSubsystem* Subsystem = GetSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		BroadcastError(FEverMemOSError::NetworkError(TEXT("EverMemOS Subsystem not available")));
		return;
	}

	Subsystem->GetConversationMetadata(GroupId, FOnConvoMetaComplete::CreateLambda(
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

