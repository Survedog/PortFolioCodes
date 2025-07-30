// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CPPlayerAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "Character/CPPlayerCharacter.h"
#include "GASTag/CPGameplayTag.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

UCPPlayerAnimInstance::UCPPlayerAnimInstance()
{
	bIsAiming = false;
}

void UCPPlayerAnimInstance::SetCharacterASC(UAbilitySystemComponent* InASC)
{
	Super::SetCharacterASC(InASC);
	CP_NETCUSTOMLOG(GetOwningActor(), LogCP, Log, TEXT("Start"));

	if (CharacterASC)
	{
		CharacterASC->RegisterGameplayTagEvent(GASTAG_CHARACTER_ACTION_Aim).AddUObject(this, &UCPPlayerAnimInstance::AimTagChangeEventCallback);
		CharacterASC->RegisterGameplayTagEvent(GASTAG_CHARACTER_ACTION_Charge).AddUObject(this, &UCPPlayerAnimInstance::ChargeTagChangeEventCallback);
	}
}

void UCPPlayerAnimInstance::NativeInitializeAnimation()
{
	CP_NETCUSTOMLOG(GetOwningActor(), LogCP, Log, TEXT("Start"));
	Super::NativeInitializeAnimation();

	CPPlayerCharacter = Cast<ACPPlayerCharacter>(GetOwningActor());
	if (CPPlayerCharacter.IsValid())
	{
		UCharacterWeaponComponent* WeaponComponent = CPPlayerCharacter->GetCharacterWeaponComponent();
		if (WeaponComponent)
		{
			WeaponComponent->OnSwapCurrentWeapon.AddUObject(this, &UCPPlayerAnimInstance::SwapCurrentWeaponCallback);
		}
	}
}

void UCPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bIsAiming && CPPlayerCharacter.IsValid())
	{
		AimRotationPitch = CPPlayerCharacter->GetAimRotationPitch();
	}
}

void UCPPlayerAnimInstance::AimTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount)
{
	bIsAiming = NewTagCount > 0;
}

void UCPPlayerAnimInstance::ChargeTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount)
{
	bIsCharging = NewTagCount > 0;
}

void UCPPlayerAnimInstance::SwapCurrentWeaponCallback(uint32 NewCurrentWeaponIndex)
{
	CP_NETCUSTOMLOG(GetOwningActor(), LogCP, Log, TEXT("Start"));
	if (CPPlayerCharacter.IsValid())
	{
		const UCharacterWeaponComponent* WeaponComponent = CPPlayerCharacter->GetCharacterWeaponComponent();
		const UWeapon* NewWeapon = WeaponComponent->GetWeaponInventory()[NewCurrentWeaponIndex];
		EquippedWeaponType = NewWeapon ? NewWeapon->GetWeaponType() : EWeaponType::None;
	}
}
