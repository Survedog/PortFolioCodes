// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/CPGameplayAbility.h"
#include "GASTag/CPGameplayTag.h"
#include "AbilitySystemComponent.h"

UCPGameplayAbility::UCPGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = false;

	ActivationBlockedTags.AddTag(GASTAG_CHARACTER_STATE_IsStunned);
	ActivationBlockedTags.AddTag(GASTAG_CHARACTER_STATE_IsKnockedDown);
	ActivationBlockedTags.AddTag(GASTAG_CHARACTER_STATE_IsDead);
}

void UCPGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
