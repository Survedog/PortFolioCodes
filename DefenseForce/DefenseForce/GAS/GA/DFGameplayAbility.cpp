// Copyright 2024, Survedog. All rights reserved.


#include "GAS/GA/DFGameplayAbility.h"
#include "DFGameplayAbility.h"
#include "GAS/DFGameplayTags.h"
#include "Interface/PlayerTowerControlInterface.h"
#include "DFLog.h"

UDFGameplayAbility::UDFGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CooldownTag = FGameplayTag::EmptyTag;
	CooldownTime = 0.0f;
}

void UDFGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (CooldownTag.IsValid() && CooldownTime > 0.0f)
	{
		GetCurrentAbilitySpec()->SetByCallerTagMagnitudes.FindOrAdd(CooldownTag, CooldownTime);
	}
}
