// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSSearchTypes.h"
#include "EverMemOSSearchAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchSuccessBP, const FEverMemOSSearchResult&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSSearchAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Search memories */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSSearchAction* SearchMemories(UObject* WorldContextObject,
		const FEverMemOSSearchParams& Params);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnSearchSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSSearchParams SearchParams;
};
