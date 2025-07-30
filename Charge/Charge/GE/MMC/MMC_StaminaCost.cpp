// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/MMC/MMC_StaminaCost.h"
#include "Interface/CPStaminaCostAbilityInterface.h"

float UMMC_StaminaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Do not call super function.
	const UGameplayAbility* Ability = Spec.GetContext().GetAbilityInstance_NotReplicated();
	const ICPStaminaCostAbilityInterface* AbilityCostInterface = Cast<ICPStaminaCostAbilityInterface>(Ability);
	const float StaminaCost = AbilityCostInterface ? AbilityCostInterface->GetStaminaCost().GetValueAtLevel(Ability->GetAbilityLevel()) : 1.0f;
	
	return -StaminaCost;
}
