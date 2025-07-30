// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TimerWeapon.h"
#include "Character/CPCharacterBase.h"
#include "Charge.h"

UTimerWeapon::UTimerWeapon()
{
	bDoPeriodicalAttack = false;
}

void UTimerWeapon::StartComboAttack_Implementation(float InChargeRate)
{
	Super::StartComboAttack_Implementation(InChargeRate);

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	if (bDoPeriodicalAttack)
	{
		const float AttackPeriod = GetAttackPeriod(InChargeRate);
		CPCharacterOwner->GetWorldTimerManager().SetTimer(AttackPeriodTimerHandle, this, &UTimerWeapon::Attack, AttackPeriod, true);
	}
	const float AttackDuration = GetAttackDuration(InChargeRate);
	CPCharacterOwner->GetWorldTimerManager().SetTimer(AttackEndTimerHandle, this, &UTimerWeapon::EndPrimaryAttack, AttackDuration, false);
}

void UTimerWeapon::EndPrimaryAttack_Implementation()
{
	Super::EndPrimaryAttack_Implementation();

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	AttackEndTimerHandle.Invalidate();
	CPCharacterOwner->GetWorldTimerManager().ClearTimer(AttackPeriodTimerHandle);
}

void UTimerWeapon::Attack()
{
	PrimaryAttack(1);
}

float UTimerWeapon::GetAttackDuration_Implementation(float InChargeRate)
{
	return InChargeRate * 10.0f;
}

float UTimerWeapon::GetAttackPeriod_Implementation(float InChargeRate)
{
	return 1.0f;
}
