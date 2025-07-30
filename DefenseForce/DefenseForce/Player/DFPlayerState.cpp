// Copyright 2024, Survedog. All rights reserved.


#include "Player/DFPlayerState.h"
#include "AbilitySystemComponent.h"

ADFPlayerState::ADFPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	NetUpdateFrequency = 50.0f;
}

UAbilitySystemComponent* ADFPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
