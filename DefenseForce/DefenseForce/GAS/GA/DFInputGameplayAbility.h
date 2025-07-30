// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/DFGameplayAbility.h"
#include "DFInputGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UDFInputGameplayAbility : public UDFGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnInputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
};
