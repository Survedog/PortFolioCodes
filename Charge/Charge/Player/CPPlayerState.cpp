// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CPStaminaAttributeSet.h"
#include "Attribute/CPChargeAttributeSet.h"
#include "Attribute/CPWeaponAttributeSet.h"

ACPPlayerState::ACPPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	CharacterBaseAttributeSet = CreateDefaultSubobject<UCPCharacterBaseAttributeSet>(TEXT("CharacterBaseAttributeSet"));
	StaminaAttributeSet = CreateDefaultSubobject<UCPStaminaAttributeSet>(TEXT("StaminaAttributeSet"));
	ChargeAttributeSet = CreateDefaultSubobject<UCPChargeAttributeSet>(TEXT("ChargeAttributeSet"));
	WeaponAttributeSet = CreateDefaultSubobject<UCPWeaponAttributeSet>(TEXT("WeaponAttributeSet"));

	SetNetUpdateFrequency(50.0f);
}

UAbilitySystemComponent* ACPPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
