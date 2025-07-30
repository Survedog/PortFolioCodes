// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Interact.h"
#include "Interface/CPInteractiveObjectInterface.h"
#include "Character/CPPlayerCharacter.h"
#include "Charge.h"

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (HasAuthority(&ActivationInfo))
	{
		ACPPlayerCharacter* CPPlayerCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
		ICPInteractiveObjectInterface* InteractTarget = CPPlayerCharacter->GetInteractTarget();
		if (InteractTarget)
		{
			AActor* TargetActor = CastChecked<AActor>(InteractTarget);
			InteractTarget->Execute_OnInteract(TargetActor, CPPlayerCharacter);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}