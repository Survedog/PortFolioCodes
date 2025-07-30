// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CPCharacterAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCPCharacterAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHARGE_API ICPCharacterAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class UWeapon* GetCurrentWeapon() = 0;
	virtual FVector GetAttackerLocation() = 0;
	virtual FRotator GetAttackerRotation() = 0;
	virtual FRotator GetAttackerControlRotation() = 0;

	virtual void SetAttackerLocation(FVector NewLocation) = 0;
	virtual void SetAttackerRotation(FRotator NewRotation) = 0;
	virtual void SetAttackerControlRotation(FRotator NewControlRotation) = 0;
};
