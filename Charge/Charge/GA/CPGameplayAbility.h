// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CPGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CHARGE_API UCPGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCPGameplayAbility();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	uint8 bActivateOnGranted : 1 = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
