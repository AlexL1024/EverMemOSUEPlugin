// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EverMemOSEnums.generated.h"

UENUM(BlueprintType)
enum class EEverMemOSMemoryType : uint8
{
	Profile UMETA(DisplayName = "Profile"),
	EpisodicMemory UMETA(DisplayName = "Episodic Memory"),
	Foresight UMETA(DisplayName = "Foresight"),
	EventLog UMETA(DisplayName = "Event Log"),
	GroupProfile UMETA(DisplayName = "Group Profile")
};

UENUM(BlueprintType)
enum class EEverMemOSRetrieveMethod : uint8
{
	Keyword UMETA(DisplayName = "Keyword"),
	Vector UMETA(DisplayName = "Vector"),
	Hybrid UMETA(DisplayName = "Hybrid"),
	RRF UMETA(DisplayName = "RRF"),
	Agentic UMETA(DisplayName = "Agentic")
};

UENUM(BlueprintType)
enum class EEverMemOSSceneType : uint8
{
	GroupChat UMETA(DisplayName = "Group Chat"),
	Assistant UMETA(DisplayName = "Assistant")
};

UENUM(BlueprintType)
enum class EEverMemOSMessageRole : uint8
{
	User UMETA(DisplayName = "User"),
	Assistant UMETA(DisplayName = "Assistant")
};
