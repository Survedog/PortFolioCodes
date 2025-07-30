// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CPEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API ACPEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACPEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void RunAI();

	UFUNCTION(BlueprintCallable)
	void StopAI();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class UBlackboardData> BlackBoardAsset;
};
