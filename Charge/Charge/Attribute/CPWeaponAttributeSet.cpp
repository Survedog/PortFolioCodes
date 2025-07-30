// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/CPWeaponAttributeSet.h"
#include "Net/UnrealNetwork.h"

UCPWeaponAttributeSet::UCPWeaponAttributeSet() : Damage(0.0f), StunValue(0.0f), MeeleAttackRangeMultiplier(1.0f)
{
}

void UCPWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UCPWeaponAttributeSet, MeeleAttackRangeMultiplier, COND_None, REPNOTIFY_Always);
}

void UCPWeaponAttributeSet::OnRep_MeeleAttackRangeMultiplier(const FGameplayAttributeData& OldMeeleAttackRangeMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPWeaponAttributeSet, MeeleAttackRangeMultiplier, OldMeeleAttackRangeMultiplier);
}
