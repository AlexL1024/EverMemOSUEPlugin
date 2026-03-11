// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Auth/EverMemOSBearerAuth.h"

FEverMemOSBearerAuth::FEverMemOSBearerAuth(const FString& InToken)
	: Token(InToken)
{
}

void FEverMemOSBearerAuth::ApplyAuth(FHttpRequestRef Request)
{
	if (!Token.IsEmpty())
	{
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
	}
}

bool FEverMemOSBearerAuth::IsConfigured() const
{
	return !Token.IsEmpty();
}

void FEverMemOSBearerAuth::SetToken(const FString& InToken)
{
	Token = InToken;
}
