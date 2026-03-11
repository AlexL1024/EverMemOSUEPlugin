// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSConvoMetaTypes.h"
#include "EverMemOSConvoMetaSetAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConvoMetaSetSuccessBP, const FEverMemOSConvoMetaData&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSConvoMetaSetAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Conversation",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSConvoMetaSetAction* SetConversationMetadata(UObject* WorldContextObject, const FEverMemOSConvoMetaUpsertRequest& Request);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnConvoMetaSetSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSConvoMetaUpsertRequest UpsertRequest;
};

