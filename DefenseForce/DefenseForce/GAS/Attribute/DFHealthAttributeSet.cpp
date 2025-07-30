// Copyright 2024, Survedog. All rights reserved.


#include "GAS/Attribute/DFHealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UDFHealthAttributeSet::UDFHealthAttributeSet() : MaxHp(100.0f), DamageToApply(0.0f)
{
	InitHp(MaxHp.GetBaseValue());
}

void UDFHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Don't call original super function.
	if (Data.EvaluatedData.Attribute == GetDamageToApplyAttribute())
	{
		const float NewHp = FMath::Clamp(GetHp() - GetDamageToApply(), 0.0f, GetMaxHp());
		SetHp(NewHp);
		SetDamageToApply(0.0f);
	}
}

void UDFHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Don't call original super function.
	if (Attribute == GetHpAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHp());
	}
}

void UDFHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	// Don't call original super function.
	if (Attribute == GetHpAttribute())
	{
		if (GetOwningActor()->HasAuthority() && FMath::IsNearlyZero(NewValue) && !bIsDestructed)
		{
			bIsDestructed = true;
			OnHpZero.Broadcast();
		}
	}
}

void UDFHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDFHealthAttributeSet, Hp, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UDFHealthAttributeSet, MaxHp, COND_None, REPNOTIFY_OnChanged);
}

void UDFHealthAttributeSet::OnRep_Hp(const FGameplayAttributeData& OldHp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDFHealthAttributeSet, Hp, OldHp);
}

void UDFHealthAttributeSet::OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDFHealthAttributeSet, MaxHp, OldMaxHp);
}
