// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Retry policy for HTTP requests */
class EVERMEMOSRUNTIME_API FEverMemOSRetryPolicy
{
public:
	FEverMemOSRetryPolicy(int32 InMaxRetries = 3, float InBaseDelay = 1.0f);

	/** Whether the given status code should trigger a retry */
	bool ShouldRetry(int32 StatusCode, int32 AttemptNumber) const;

	/** Get the delay before the next retry attempt (exponential backoff) */
	float GetRetryDelay(int32 AttemptNumber) const;

	int32 GetMaxRetries() const { return MaxRetries; }

private:
	int32 MaxRetries;
	float BaseDelay;
};
