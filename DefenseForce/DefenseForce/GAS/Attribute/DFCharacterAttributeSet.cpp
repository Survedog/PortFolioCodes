// Copyright 2024, Survedog. All rights reserved.


#include "GAS/Attribute/DFCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

UDFCharacterAttributeSet::UDFCharacterAttributeSet() : MoveSpeed(300.0f)
{
}

void UDFCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDFCharacterAttributeSet, MoveSpeed, COND_None, REPNOTIFY_OnChanged);
}

void UDFCharacterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDFCharacterAttributeSet, MoveSpeed, OldMoveSpeed);
}
