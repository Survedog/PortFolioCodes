// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "TimerWeapon.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UTimerWeapon : public UWeapon
{
	GENERATED_BODY()
	
public:
	UTimerWeapon();

protected:
	virtual void StartComboAttack_Implementation(float InChargeRate) override;
	virtual void EndPrimaryAttack_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	float GetAttackDuration(float InChargeRate);

	UFUNCTION(BlueprintNativeEvent)
	float GetAttackPeriod(float InChargeRate);	

private:
	void Attack();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	uint8 bDoPeriodicalAttack : 1;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle AttackPeriodTimerHandle;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle AttackEndTimerHandle;
};
