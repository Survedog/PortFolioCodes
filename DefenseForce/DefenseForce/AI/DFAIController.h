// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "DFAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFAIController : public AAIController, public IDFAttackerInfoInterface
{
	GENERATED_BODY()
	
public:
	virtual class AActor* GetAttackerActor() const override;

	virtual class UAbilitySystemComponent* GetAttackerActorASC() const override;
};
