// Copyright TonyL. All Rights Reserved.

#include "EverMemOSJsonHelper.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

FString FEverMemOSJsonHelper::PascalToSnake(const FString& PascalString)
{
	if (PascalString.IsEmpty())
	{
		return PascalString;
	}

	FString Result;
	Result.Reserve(PascalString.Len() + 4);

	for (int32 i = 0; i < PascalString.Len(); ++i)
	{
		const TCHAR Ch = PascalString[i];
		if (FChar::IsUpper(Ch))
		{
			if (i > 0)
			{
				// Don't add underscore if previous char was also uppercase and next is uppercase or end
				// e.g., "SSEReader" -> "sse_reader", not "s_s_e_reader"
				const bool bPrevUpper = FChar::IsUpper(PascalString[i - 1]);
				const bool bNextLower = (i + 1 < PascalString.Len()) && FChar::IsLower(PascalString[i + 1]);
				if (!bPrevUpper || bNextLower)
				{
					Result += TEXT('_');
				}
			}
			Result += FChar::ToLower(Ch);
		}
		else
		{
			Result += Ch;
		}
	}

	return Result;
}

FString FEverMemOSJsonHelper::SnakeToPascal(const FString& SnakeString)
{
	if (SnakeString.IsEmpty())
	{
		return SnakeString;
	}

	FString Result;
	Result.Reserve(SnakeString.Len());

	bool bCapitalizeNext = true;

	for (int32 i = 0; i < SnakeString.Len(); ++i)
	{
		const TCHAR Ch = SnakeString[i];
		if (Ch == TEXT('_'))
		{
			bCapitalizeNext = true;
		}
		else if (bCapitalizeNext)
		{
			Result += FChar::ToUpper(Ch);
			bCapitalizeNext = false;
		}
		else
		{
			Result += Ch;
		}
	}

	return Result;
}

TSharedPtr<FJsonObject> FEverMemOSJsonHelper::ConvertKeysToSnake(const TSharedPtr<FJsonObject>& InObject)
{
	if (!InObject.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<FJsonObject> OutObject = MakeShareable(new FJsonObject());

	for (const auto& Pair : InObject->Values)
	{
		const FString SnakeKey = PascalToSnake(Pair.Key);
		OutObject->SetField(SnakeKey, ConvertValueKeys(Pair.Value,
			[](const FString& Key) { return PascalToSnake(Key); }));
	}

	return OutObject;
}

TSharedPtr<FJsonObject> FEverMemOSJsonHelper::ConvertKeysToPascal(const TSharedPtr<FJsonObject>& InObject)
{
	if (!InObject.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<FJsonObject> OutObject = MakeShareable(new FJsonObject());

	for (const auto& Pair : InObject->Values)
	{
		const FString PascalKey = SnakeToPascal(Pair.Key);
		OutObject->SetField(PascalKey, ConvertValueKeys(Pair.Value,
			[](const FString& Key) { return SnakeToPascal(Key); }));
	}

	return OutObject;
}

TSharedPtr<FJsonValue> FEverMemOSJsonHelper::ConvertValueKeys(
	const TSharedPtr<FJsonValue>& InValue,
	TFunction<FString(const FString&)> KeyConverter)
{
	if (!InValue.IsValid())
	{
		return nullptr;
	}

	// Handle object values
	const TSharedPtr<FJsonObject>* ObjPtr;
	if (InValue->TryGetObject(ObjPtr) && ObjPtr && (*ObjPtr).IsValid())
	{
		TSharedPtr<FJsonObject> NewObj = MakeShareable(new FJsonObject());
		for (const auto& Pair : (*ObjPtr)->Values)
		{
			NewObj->SetField(KeyConverter(Pair.Key), ConvertValueKeys(Pair.Value, KeyConverter));
		}
		return MakeShareable(new FJsonValueObject(NewObj));
	}

	// Handle array values
	const TArray<TSharedPtr<FJsonValue>>* ArrayPtr;
	if (InValue->TryGetArray(ArrayPtr) && ArrayPtr)
	{
		TArray<TSharedPtr<FJsonValue>> NewArray;
		NewArray.Reserve(ArrayPtr->Num());
		for (const auto& Element : *ArrayPtr)
		{
			NewArray.Add(ConvertValueKeys(Element, KeyConverter));
		}
		return MakeShareable(new FJsonValueArray(NewArray));
	}

	// Primitive values pass through unchanged
	return InValue;
}
