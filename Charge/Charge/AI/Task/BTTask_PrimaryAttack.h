// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PrimaryAttack.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UBTTask_PrimaryAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PrimaryAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	TSubclassOf<class UGameplayAbility> PrimaryAttackAbilityClass;
};
