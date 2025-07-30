// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_DropWeapon.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Charge.h"

void UGA_DropWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	UCharacterWeaponComponent* CharacterWeaponComponent = ActorInfo->AvatarActor.Get()->GetComponentByClass<UCharacterWeaponComponent>();
	CharacterWeaponComponent->DropCurrentWeapon();

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
