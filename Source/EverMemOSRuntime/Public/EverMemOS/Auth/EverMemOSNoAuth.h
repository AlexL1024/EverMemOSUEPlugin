// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IEverMemOSAuthProvider.h"

/** No-op auth provider for local deployment (no credentials needed) */
class EVERMEMOSRUNTIME_API FEverMemOSNoAuth : public IEverMemOSAuthProvider
{
public:
	virtual void ApplyAuth(FHttpRequestRef Request) override {}
	virtual bool IsConfigured() const override { return true; }
};
