// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/CPChargeAttributeSet.h"
#include "Net/UnrealNetwork.h"

UCPChargeAttributeSet::UCPChargeAttributeSet() : ChargeAmount(0.0f), MaxChargeAmount(100.0f), ChargeSpeed(1.0f)
{
}

void UCPChargeAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Do not call super function.
	if (Attribute == GetChargeAmountAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxChargeAmount());
	}
}

void UCPChargeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCPChargeAttributeSet, ChargeAmount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCPChargeAttributeSet, MaxChargeAmount, COND_None, REPNOTIFY_Always);
}

void UCPChargeAttributeSet::OnRep_ChargeAmount(const FGameplayAttributeData& OldChargeAmount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPChargeAttributeSet, ChargeAmount, OldChargeAmount);
}

void UCPChargeAttributeSet::OnRep_MaxChargeAmount(const FGameplayAttributeData& OldMaxChargeAmount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPChargeAttributeSet, MaxChargeAmount, OldMaxChargeAmount);
}