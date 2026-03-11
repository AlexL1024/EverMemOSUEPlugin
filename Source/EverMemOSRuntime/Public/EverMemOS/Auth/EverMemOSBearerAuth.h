// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IEverMemOSAuthProvider.h"

class EVERMEMOSRUNTIME_API FEverMemOSBearerAuth : public IEverMemOSAuthProvider
{
public:
	explicit FEverMemOSBearerAuth(const FString& InToken);

	virtual void ApplyAuth(FHttpRequestRef Request) override;
	virtual bool IsConfigured() const override;

	void SetToken(const FString& InToken);

private:
	FString Token;
};
