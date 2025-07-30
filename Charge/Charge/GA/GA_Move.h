// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "GA_Move.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_Move : public UCPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Move();

protected:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
};
