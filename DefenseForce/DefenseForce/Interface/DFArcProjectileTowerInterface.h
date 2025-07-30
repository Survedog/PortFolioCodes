// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DFArcProjectileTowerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UDFArcProjectileTowerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEFENSEFORCE_API IDFArcProjectileTowerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector GetProjectileLaunchVelocity() const;
};
