// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDFAbilityInputID : uint8
{
	// 0 None  
	None			UMETA(DisplayName = "None"),
	// 1 TowerControl
	Targeting	UMETA(DisplayName = "Targeting"),
	// 2 Attack
	Attack			UMETA(DisplayName = "Attack"),
	// 3 BuildStructure
	BuildStructure	UMETA(DisplayName = "BuildStructure"),
};