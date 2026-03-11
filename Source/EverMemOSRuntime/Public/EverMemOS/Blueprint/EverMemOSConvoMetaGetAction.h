// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSConvoMetaTypes.h"
#include "EverMemOSConvoMetaGetAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConvoMetaGetSuccessBP, const FEverMemOSConvoMetaData&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSConvoMetaGetAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Conversation",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSConvoMetaGetAction* GetConversationMetadata(UObject* WorldContextObject, const FString& GroupId);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnConvoMetaGetSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FString GroupId;
};

