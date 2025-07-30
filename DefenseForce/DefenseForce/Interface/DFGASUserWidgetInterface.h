// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DFGASUserWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDFGASUserWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEFENSEFORCE_API IDFGASUserWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetAbilitySystemComponent(class UAbilitySystemComponent* NewASC) = 0;
};
