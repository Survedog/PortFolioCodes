// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/CPStaminaAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GASTag/CPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UCPStaminaAttributeSet::UCPStaminaAttributeSet() : MaxStamina(100.0f), StaminaRecoverySpeed(10.0f), bOutOfStamina(false), StaminaRecoveryCooldown(2.0f)
{
	InitStamina(GetMaxStamina());

	static ConstructorHelpers::FClassFinder<UGameplayEffect> RecoverStaminaEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_RecoverStamina.BPGE_RecoverStamina_C"));
	if (RecoverStaminaEffectClassRef.Class)
	{
		StaminaRecoveryEffect = RecoverStaminaEffectClassRef.Class;
	}

	ActiveStaminaRecoveryEffectHandle.Invalidate();
	StaminaRecoveryTimerHandle.Invalidate();

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOuter());
	if (ASI)
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		ASC->RegisterGameplayTagEvent(GASTAG_CHARACTER_STATE_IsConsumingStamina, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &UCPStaminaAttributeSet::OnStaminaConsumingEndCallback);
	}
}

void UCPStaminaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UCPStaminaAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetStaminaAttribute())
	{
		if (NewValue < OldValue && FMath::IsNearlyZero(NewValue) && !bOutOfStamina)
		{
			bOutOfStamina = true;
			OnOutOfStamina.Broadcast();
			if (ActiveStaminaRecoveryEffectHandle.IsValid())
			{
				GetOwningAbilitySystemComponentChecked()->RemoveActiveGameplayEffect(ActiveStaminaRecoveryEffectHandle);
				ActiveStaminaRecoveryEffectHandle.Invalidate();
			}
		}
		else if (NewValue > OldValue)
		{
			if (bOutOfStamina)
			{
				bOutOfStamina = false;
			}

			if (FMath::IsNearlyEqual(NewValue, GetMaxStamina()))
			{
				if (ActiveStaminaRecoveryEffectHandle.IsValid())
				{
					GetOwningAbilitySystemComponentChecked()->RemoveActiveGameplayEffect(ActiveStaminaRecoveryEffectHandle);
					ActiveStaminaRecoveryEffectHandle.Invalidate();
				}
			}
		}
	}
}

void UCPStaminaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCPStaminaAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCPStaminaAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCPStaminaAttributeSet, StaminaRecoverySpeed, COND_None, REPNOTIFY_Always);
}

void UCPStaminaAttributeSet::OnStaminaConsumingEndCallback(const FGameplayTag Tag, int32 NewTagCount)
{
	if (NewTagCount == 0)
	{
		if (bOutOfStamina && !StaminaRecoveryTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryTimerHandle, this, &UCPStaminaAttributeSet::StartStaminaRecovery, StaminaRecoveryCooldown, false);
		}
		else
		{
			StartStaminaRecovery();
		}
	}
}

void UCPStaminaAttributeSet::StartStaminaRecovery()
{
	StaminaRecoveryTimerHandle.Invalidate();
	if (!ActiveStaminaRecoveryEffectHandle.IsValid())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(StaminaRecoveryEffect, 1.0f, FGameplayEffectContextHandle());
		if (EffectSpecHandle.IsValid())
		{
			ActiveStaminaRecoveryEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}

void UCPStaminaAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPStaminaAttributeSet, Stamina, OldStamina);
}

void UCPStaminaAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPStaminaAttributeSet, MaxStamina, OldMaxStamina);
}

void UCPStaminaAttributeSet::OnRep_StaminaRecoverySpeed(const FGameplayAttributeData& OldStaminaRecoverySpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPStaminaAttributeSet, StaminaRecoverySpeed, OldStaminaRecoverySpeed);
}