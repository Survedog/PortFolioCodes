// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DFGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentMoneyAmountChangeDelegate, float, CurrentMoneyAmount);

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ADFGameState();

	FORCEINLINE float GetCurrentMoneyAmount() const { return CurrentMoneyAmount; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SpendMoney(float InSpendMoneyAmount) { CurrentMoneyAmount -= InSpendMoneyAmount; OnCurrentMoneyAmountChange.Broadcast(CurrentMoneyAmount); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void EarnMoney(float InEarnedMoneyAmount) { CurrentMoneyAmount += InEarnedMoneyAmount; OnCurrentMoneyAmountChange.Broadcast(CurrentMoneyAmount); }	

public:
	UPROPERTY(BlueprintAssignable)
	FOnCurrentMoneyAmountChangeDelegate OnCurrentMoneyAmountChange;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentMoneyAmount();

	UFUNCTION()
	void OnRep_IsGameOver();

	UFUNCTION(BlueprintCallable)
	void SetGameOver() { bIsGameOver = true; OnGameOverCallback(); };

	UFUNCTION(BlueprintNativeEvent)
	void OnGameOverCallback();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentMoneyAmount, Category="Resource")
	float CurrentMoneyAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsGameOver, Category="Game")
	uint8 bIsGameOver;
};
