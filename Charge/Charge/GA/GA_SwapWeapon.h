// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPParamGameplayAbility.h"
#include "InputActionValue.h"
#include "GA_SwapWeapon.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_SwapWeapon : public UCPParamGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SwapWeapon();

protected:
	virtual void InputActionPressed(const FInputActionValue& Value) override;

	UFUNCTION(Server, Reliable)
	void ServerRPCSwapCurrentWeapon(uint32 NewCurrentWeaponIndex);
};
