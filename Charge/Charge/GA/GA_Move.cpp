// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Move.h"
#include "Charge.h"

UGA_Move::UGA_Move()
{
	bServerRespectsRemoteAbilityCancellation = true;
}

void UGA_Move::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
