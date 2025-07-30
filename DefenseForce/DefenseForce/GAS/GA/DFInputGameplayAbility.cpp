// Copyright 2024, Survedog. All rights reserved.


#include "GAS/GA/DFInputGameplayAbility.h"

void UDFInputGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	OnInputPressed(Handle, *ActorInfo, ActivationInfo);
}

void UDFInputGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	OnInputReleased(Handle, *ActorInfo, ActivationInfo);
}
