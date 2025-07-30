// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_SwapWeapon.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Charge.h"

UGA_SwapWeapon::UGA_SwapWeapon()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	bBindInputPressed = true;
	bBindInputReleased = false;
}

// The actual weapon swap occurs here.
void UGA_SwapWeapon::InputActionPressed(const FInputActionValue& Value)
{
	// Do not call super function.
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	uint32 SwapWeaponIndex = FMath::RoundToInt32(Value.Get<float>()) - 1;
	ServerRPCSwapCurrentWeapon(SwapWeaponIndex);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_SwapWeapon::ServerRPCSwapCurrentWeapon_Implementation(uint32 NewCurrentWeaponIndex)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	UCharacterWeaponComponent* CharacterWeaponComponent = CurrentActorInfo->AvatarActor.Get()->GetComponentByClass<UCharacterWeaponComponent>();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Ensured();
	CharacterWeaponComponent->SwapCurrentWeapon(NewCurrentWeaponIndex, ASC);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
