// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSDeleteMemoriesTypes.h"
#include "EverMemOSDeleteMemoriesAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeleteMemoriesSuccessBP, const FEverMemOSDeleteMemoriesResult&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSDeleteMemoriesAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Delete memories (DELETE /memories) */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSDeleteMemoriesAction* DeleteMemories(UObject* WorldContextObject,
		const FEverMemOSDeleteMemoriesRequest& Request);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnDeleteMemoriesSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSDeleteMemoriesRequest DeleteRequest;
};

