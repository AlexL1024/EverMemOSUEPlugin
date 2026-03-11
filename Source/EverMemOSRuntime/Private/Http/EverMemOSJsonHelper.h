// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

/** Utility for converting between PascalCase (UE convention) and snake_case (API convention) */
class FEverMemOSJsonHelper
{
public:
	/** Convert PascalCase to snake_case (e.g., "UserId" -> "user_id") */
	static FString PascalToSnake(const FString& PascalString);

	/** Convert snake_case to PascalCase (e.g., "user_id" -> "UserId") */
	static FString SnakeToPascal(const FString& SnakeString);

	/** Convert all keys in a JSON object from PascalCase to snake_case */
	static TSharedPtr<FJsonObject> ConvertKeysToSnake(const TSharedPtr<FJsonObject>& InObject);

	/** Convert all keys in a JSON object from snake_case to PascalCase */
	static TSharedPtr<FJsonObject> ConvertKeysToPascal(const TSharedPtr<FJsonObject>& InObject);

	/** Convert a JSON value recursively (handles nested objects and arrays) */
	static TSharedPtr<FJsonValue> ConvertValueKeys(const TSharedPtr<FJsonValue>& InValue,
		TFunction<FString(const FString&)> KeyConverter);
};
