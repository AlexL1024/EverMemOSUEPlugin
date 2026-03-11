// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSGetMemoriesTypes.h"
#include "EverMemOSGetMemoriesAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetMemoriesSuccessBP, const FEverMemOSGetMemoriesResult&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSGetMemoriesAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Get memories (GET /memories) */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSGetMemoriesAction* GetMemories(UObject* WorldContextObject,
		const FEverMemOSGetMemoriesParams& Params);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnGetMemoriesSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSGetMemoriesParams GetParams;
};

