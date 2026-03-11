// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EverMemOS/EverMemOSErrors.h"
#include "EverMemOSAsyncActionBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEverMemOSErrorBP, const FEverMemOSError&, Error);

class UEverMemOSSubsystem;

/** Base class for EverMemOS async Blueprint actions */
UCLASS(Abstract)
class EVERMEMOSRUNTIME_API UEverMemOSAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Error output pin - fires on any failure */
	UPROPERTY(BlueprintAssignable, Category = "EverMemOS")
	FOnEverMemOSErrorBP OnFailure;

protected:
	/** Get the EverMemOS subsystem from the world context */
	UEverMemOSSubsystem* GetSubsystem(const UObject* WorldContextObject) const;

	/** Broadcast error and cleanup */
	void BroadcastError(const FEverMemOSError& Error);
};
