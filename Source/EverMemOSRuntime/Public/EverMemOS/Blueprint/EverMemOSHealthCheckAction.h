// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSAsyncActionBase.h"
#include "EverMemOS/Models/EverMemOSHealthTypes.h"
#include "EverMemOSHealthCheckAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthCheckSuccessBP, const FEverMemOSHealthResponse&, Response);

UCLASS(meta = (BlueprintInternalUseOnly = "true"))
class EVERMEMOSRUNTIME_API UEverMemOSHealthCheckAction : public UEverMemOSAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Check the health of the EverMemOS API */
	UFUNCTION(BlueprintCallable, Category = "EverMemOS|System",
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UEverMemOSHealthCheckAction* HealthCheck(UObject* WorldContextObject);

	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnHealthCheckSuccessBP OnSuccess;

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
};
