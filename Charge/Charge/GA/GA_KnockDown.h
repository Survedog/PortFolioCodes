// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "GA_KnockDown.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_KnockDown : public UCPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_KnockDown();

	void EnableGettingUp();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;	

	UFUNCTION()
	void OnMoveTagAddedCallback();

protected:
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = GAS)
	FActiveGameplayEffectHandle ActiveInvincibleEffectHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=GAS)
	TSubclassOf<class UGameplayEffect> InvincibleEffectClass;
};
