// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Http/EverMemOSRetryPolicy.h"

FEverMemOSRetryPolicy::FEverMemOSRetryPolicy(int32 InMaxRetries, float InBaseDelay)
	: MaxRetries(InMaxRetries)
	, BaseDelay(InBaseDelay)
{
}

bool FEverMemOSRetryPolicy::ShouldRetry(int32 StatusCode, int32 AttemptNumber) const
{
	if (AttemptNumber >= MaxRetries)
	{
		return false;
	}

	// Retry on 429 (rate limit) and 5xx (server errors)
	return StatusCode == 429 || StatusCode >= 500;
}

float FEverMemOSRetryPolicy::GetRetryDelay(int32 AttemptNumber) const
{
	// Exponential backoff: BaseDelay * 2^attempt
	return BaseDelay * FMath::Pow(2.0f, static_cast<float>(AttemptNumber));
}
