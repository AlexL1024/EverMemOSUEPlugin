// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

/** Abstract interface for authentication strategies */
class EVERMEMOSRUNTIME_API IEverMemOSAuthProvider
{
public:
	virtual ~IEverMemOSAuthProvider() = default;

	/** Apply authentication headers to the HTTP request */
	virtual void ApplyAuth(FHttpRequestRef Request) = 0;

	/** Whether this provider has valid credentials configured */
	virtual bool IsConfigured() const = 0;
};
