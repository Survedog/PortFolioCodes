// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CPAIEnemyInterface.generated.h"

DECLARE_DELEGATE_OneParam(FOnAIEnemyAttackDelegate, bool /*bIsAttackHit*/);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCPAIEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHARGE_API ICPAIEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual APawn* GetAttackTarget() = 0;
	virtual void SetAttackTarget(APawn* InAttackTarget) = 0;
};
