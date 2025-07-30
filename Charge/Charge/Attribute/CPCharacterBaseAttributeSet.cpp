// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/CPCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GASTag/CPGameplayTag.h"
#include "Game/CPDamageTypeSubsystem.h"
#include "Engine/DamageEvents.h"
#include "Charge.h"

UCPCharacterBaseAttributeSet::UCPCharacterBaseAttributeSet() : 
	MaxHp(100.0f),
	DamageToApply(0.0f),
	StunGauge(0.0f),
	MaxStunGauge(100.0f),
	HitInvincibleTime(0.1f),
	MoveSpeed(500.0f),
	bIsDead(false)
{
	InitHp(GetMaxHp());

	static ConstructorHelpers::FClassFinder<UGameplayEffect> InvincibleEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_Invincible_HitDelay.BPGE_Invincible_HitDelay_C"));
	if (InvincibleEffectClassRef.Class)
	{
		InvincibleEffect = InvincibleEffectClassRef.Class;
	}
}

void UCPCharacterBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Don't call super function.
	if (Data.EvaluatedData.Attribute == GetDamageToApplyAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
		const float DamageAmount = GetDamageToApply();
		const float NewHp = FMath::Clamp(GetHp() - DamageAmount, 0.0f, GetMaxHp());

		ACPCharacterBase* CPCharacterOwner = Cast<ACPCharacterBase>(ASC->GetAvatarActor());
		if (CPCharacterOwner)
		{
			APawn* DamageInstigatorPawn = Cast<APawn>(Data.EffectSpec.GetEffectContext().GetInstigator());
			if (DamageInstigatorPawn)
			{
				AController* DamageInstigatorController = DamageInstigatorPawn->GetController();
				CPCharacterOwner->MulticastRPCSpawnDamageIndicator(DamageAmount, DamageInstigatorController);
			}
		}

		SetHp(NewHp);
		SetDamageToApply(0.0f);
		ApplyInvincibleEffect(GetHitInvincibleTime());

		CP_NETATTLOG(LogCPGAS, Log, TEXT("%s dealt damage to %s. Damage: %f / Hp: %f -> %f"), *ASC->GetAvatarActor()->GetActorLabel(), *Data.Target.GetAvatarActor()->GetActorLabel(), DamageAmount, GetHp(), NewHp);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHpAttribute())
	{
		if (GetHp() > GetMaxHp())
		{
			SetHp(GetMaxHp());
		}
	}

	if (GetOwningActor()->HasAuthority() && FMath::IsNearlyZero(GetHp()) && !bIsDead)
	{
		bIsDead = true;
		OnHpZero.Broadcast();
	}
}

void UCPCharacterBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Don't call super function.
	if (Attribute == GetDamageToApplyAttribute())
	{
		NewValue = NewValue >= 0.0f ? NewValue : 0.0f;
	}
}

void UCPCharacterBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCPCharacterBaseAttributeSet, Hp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCPCharacterBaseAttributeSet, MaxHp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCPCharacterBaseAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UCPCharacterBaseAttributeSet::OnRep_Hp(const FGameplayAttributeData& OldHp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPCharacterBaseAttributeSet, Hp, OldHp);
}

void UCPCharacterBaseAttributeSet::OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPCharacterBaseAttributeSet, MaxHp, OldMaxHp);
}

void UCPCharacterBaseAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCPCharacterBaseAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UCPCharacterBaseAttributeSet::ApplyInvincibleEffect(float InvincibleTime)
{
	if (InvincibleTime > 0.0f)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponentChecked();
		FGameplayEffectSpecHandle InvincibleEffectSpecHandle = ASC->MakeOutgoingSpec(InvincibleEffect, 1.0f, FGameplayEffectContextHandle());
		InvincibleEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_CHARACTER_STATE_Invincible, InvincibleTime);
		ASC->BP_ApplyGameplayEffectSpecToSelf(InvincibleEffectSpecHandle);
	}
}
