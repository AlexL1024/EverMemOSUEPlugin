// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSAddMemoryTypes.h"
#include "EverMemOSAddMemoryAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddMemorySuccessBP, const FEverMemOSAddMemoryResponse&, Result);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSAddMemoryAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Add a memory (POST /memories) */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|Memory",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSAddMemoryAction* AddMemory(UObject* WorldContextObject,
		const FEverMemOSAddMemoryRequest& Request);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnAddMemorySuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FEverMemOSAddMemoryRequest AddRequest;
};

