// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerTowerControlInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPlayerTowerControlInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEFENSEFORCE_API IPlayerTowerControlInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual void StartTowerControl(class ADFTowerBase* NewTower) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void EndTowerControl() = 0;

	UFUNCTION(BlueprintCallable)
	virtual class ADFTowerBase* GetCurrentControlledTower() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual class ADFStructureBase* GetCurrentStructureUnderCursor() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetPlayerAimLocation() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetPlayerAimLocation(const FVector& InPlayerAimLocation) = 0;
};
