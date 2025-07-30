// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerBuildModeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPlayerBuildModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEFENSEFORCE_API IPlayerBuildModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual void EnterBuildMode(TSubclassOf<class ADFStructureBase> InTargetStructureClass) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ExitBuildMode(class ADFStructureBase* InBuiltStructure) = 0;
};
