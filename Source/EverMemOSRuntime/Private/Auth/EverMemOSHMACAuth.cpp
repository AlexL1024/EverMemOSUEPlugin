// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/Auth/EverMemOSHMACAuth.h"
#include "EverMemOS/EverMemOSLog.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/Guid.h"

FEverMemOSHMACAuth::FEverMemOSHMACAuth(const FString& InSecretKey)
	: SecretKey(InSecretKey)
{
}

void FEverMemOSHMACAuth::ApplyAuth(FHttpRequestRef Request)
{
	if (SecretKey.IsEmpty())
	{
		UE_LOG(LogEverMemOS, Warning, TEXT("HMAC secret key is empty, skipping auth"));
		return;
	}

	const FString Method = Request->GetVerb().ToUpper();

	// Extract path from URL (without query string)
	FString URL = Request->GetURL();
	FString Path;
	FString QueryString;
	// Remove scheme and host
	int32 SchemeEnd = URL.Find(TEXT("://"));
	if (SchemeEnd != INDEX_NONE)
	{
		FString AfterScheme = URL.Mid(SchemeEnd + 3);
		int32 PathStart = AfterScheme.Find(TEXT("/"));
		if (PathStart != INDEX_NONE)
		{
			FString PathAndQuery = AfterScheme.Mid(PathStart);
			int32 QueryStart = PathAndQuery.Find(TEXT("?"));
			if (QueryStart != INDEX_NONE)
			{
				Path = PathAndQuery.Left(QueryStart);
			}
			else
			{
				Path = PathAndQuery;
			}
		}
		else
		{
			Path = TEXT("/");
		}
	}
	else
	{
		Path = TEXT("/");
	}

	const FString Timestamp = FString::Printf(TEXT("%lld"), FDateTime::UtcNow().ToUnixTimestamp());
	const FString Nonce = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphensLower);

	// Signature string: METHOD|PATH|TIMESTAMP|NONCE
	const FString SignatureString = FString::Printf(TEXT("%s|%s|%s|%s"), *Method, *Path, *Timestamp, *Nonce);
	const FString Signature = ComputeHMACSHA256(SecretKey, SignatureString);

	Request->SetHeader(TEXT("X-Timestamp"), Timestamp);
	Request->SetHeader(TEXT("X-Nonce"), Nonce);
	Request->SetHeader(TEXT("X-Signature"), Signature);
}

bool FEverMemOSHMACAuth::IsConfigured() const
{
	return !SecretKey.IsEmpty();
}

void FEverMemOSHMACAuth::SetSecretKey(const FString& InSecretKey)
{
	SecretKey = InSecretKey;
}

FString FEverMemOSHMACAuth::ComputeHMACSHA256(const FString& InKey, const FString& InData)
{
	const FTCHARToUTF8 KeyUTF8(*InKey);
	const FTCHARToUTF8 DataUTF8(*InData);

	TArray<uint8> KeyBytes;
	KeyBytes.Append(reinterpret_cast<const uint8*>(KeyUTF8.Get()), KeyUTF8.Length());

	constexpr int32 BlockSize = 64; // SHA-256 block size
	TArray<uint8> KeyBlock;

	if (KeyBytes.Num() > BlockSize)
	{
		FSHA256Signature KeyHash;
		if (!FPlatformMisc::GetSHA256Signature(KeyBytes.GetData(), static_cast<uint32>(KeyBytes.Num()), KeyHash))
		{
			UE_LOG(LogEverMemOS, Error, TEXT("Failed to compute SHA256 for HMAC key"));
			return FString();
		}
		KeyBlock.Append(KeyHash.Signature, UE_ARRAY_COUNT(KeyHash.Signature));
	}
	else
	{
		KeyBlock = MoveTemp(KeyBytes);
	}

	// Pad key to block size
	if (KeyBlock.Num() < BlockSize)
	{
		const int32 OldNum = KeyBlock.Num();
		KeyBlock.SetNum(BlockSize, EAllowShrinking::No);
		FMemory::Memzero(KeyBlock.GetData() + OldNum, BlockSize - OldNum);
	}
	else if (KeyBlock.Num() > BlockSize)
	{
		KeyBlock.SetNum(BlockSize, EAllowShrinking::No);
	}

	TArray<uint8> Ipad;
	TArray<uint8> Opad;
	Ipad.SetNumUninitialized(BlockSize);
	Opad.SetNumUninitialized(BlockSize);

	for (int32 i = 0; i < BlockSize; ++i)
	{
		Ipad[i] = KeyBlock[i] ^ 0x36;
		Opad[i] = KeyBlock[i] ^ 0x5c;
	}

	// Inner hash = SHA256(ipad || data)
	TArray<uint8> InnerData;
	InnerData.Reserve(BlockSize + DataUTF8.Length());
	InnerData.Append(Ipad);
	InnerData.Append(reinterpret_cast<const uint8*>(DataUTF8.Get()), DataUTF8.Length());

	FSHA256Signature InnerHash;
	if (!FPlatformMisc::GetSHA256Signature(InnerData.GetData(), static_cast<uint32>(InnerData.Num()), InnerHash))
	{
		UE_LOG(LogEverMemOS, Error, TEXT("Failed to compute inner SHA256 for HMAC"));
		return FString();
	}

	// Outer hash = SHA256(opad || innerHash)
	TArray<uint8> OuterData;
	OuterData.Reserve(BlockSize + UE_ARRAY_COUNT(InnerHash.Signature));
	OuterData.Append(Opad);
	OuterData.Append(InnerHash.Signature, UE_ARRAY_COUNT(InnerHash.Signature));

	FSHA256Signature OuterHash;
	if (!FPlatformMisc::GetSHA256Signature(OuterData.GetData(), static_cast<uint32>(OuterData.Num()), OuterHash))
	{
		UE_LOG(LogEverMemOS, Error, TEXT("Failed to compute outer SHA256 for HMAC"));
		return FString();
	}

	return OuterHash.ToString();
}
