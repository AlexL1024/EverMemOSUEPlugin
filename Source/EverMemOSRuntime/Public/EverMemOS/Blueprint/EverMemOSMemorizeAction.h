// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSMemorizeTypes.h"
#include "EverMemOSMemorizeAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemorizeSuccessBP, const FEverMemOSMemorizeResult&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSMemorizeAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Memorize messages into the memory system */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSMemorizeAction* Memorize(UObject* WorldContextObject,
		const FEverMemOSMemorizeRequest& Request);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnMemorizeSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSMemorizeRequest MemorizeRequest;
};
