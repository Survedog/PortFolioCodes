// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector Origin;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector OutRandomLocation;

	UPROPERTY(EditAnywhere, Category = Node)
	float Radius;
};
