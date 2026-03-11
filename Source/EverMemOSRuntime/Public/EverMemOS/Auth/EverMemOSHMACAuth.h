// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IEverMemOSAuthProvider.h"

class EVERMEMOSRUNTIME_API FEverMemOSHMACAuth : public IEverMemOSAuthProvider
{
public:
	explicit FEverMemOSHMACAuth(const FString& InSecretKey);

	virtual void ApplyAuth(FHttpRequestRef Request) override;
	virtual bool IsConfigured() const override;

	void SetSecretKey(const FString& InSecretKey);

	/**
	 * Compute HMAC-SHA256 signature.
	 * SignatureString format: METHOD|PATH|TIMESTAMP|NONCE
	 * Returns lowercase hex-encoded string (64 chars).
	 */
	static FString ComputeHMACSHA256(const FString& InKey, const FString& InData);

private:
	FString SecretKey;
};
