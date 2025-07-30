// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Stun.h"
#include "Character/CPCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "GASTag/CPGameplayTag.h"
#include "Charge.h"

UGA_Stun::UGA_Stun()
{
	ActivationBlockedTags.RemoveTag(GASTAG_CHARACTER_STATE_IsStunned);
	bRetriggerInstancedAbility = true;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	static ConstructorHelpers::FClassFinder<UGameplayEffect> InitStunGaugeEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_InitStunGauge.BPGE_InitStunGauge_C"));
	if (InitStunGaugeEffectClassRef.Class)
	{
		InitStunGaugeEffect = InitStunGaugeEffectClassRef.Class;
	}
}

bool UGA_Stun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Ensured();
	const UCPCharacterBaseAttributeSet* BaseAttributeSet = ASC->GetSet<UCPCharacterBaseAttributeSet>();
	if (BaseAttributeSet)
	{
		const float CurrentStunGauge = BaseAttributeSet->GetStunGauge();
		const float MaxStunGauge = BaseAttributeSet->GetMaxStunGauge();
		return CurrentStunGauge >= MaxStunGauge;
	}
	return false;
}

void UGA_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);	

	ACPCharacterBase* CPCharacter = CastChecked<ACPCharacterBase>(ActorInfo->AvatarActor);
	const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult();
	CPCharacter->SetActorRotation(HitResult->ImpactNormal.GetSafeNormal2D().ToOrientationQuat());
	CPCharacter->SetIgnoreMovement(true);

	UAbilityTask_PlayMontageAndWait* PlayStunMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayStunMontage"), CPCharacter->GetStunnedMontage());
	PlayStunMontageTask->OnCompleted.AddDynamic(this, &UGA_Stun::OnCompletedCallback);
	PlayStunMontageTask->OnInterrupted.AddDynamic(this, &UGA_Stun::OnInterruptedCallback);
	PlayStunMontageTask->ReadyForActivation();	

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(InitStunGaugeEffect);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
}

void UGA_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACPCharacterBase* CPCharacter = CastChecked<ACPCharacterBase>(ActorInfo->AvatarActor);
	CPCharacter->SetIgnoreMovement(false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Stun::OnCompletedCallback()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Stun::OnInterruptedCallback()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
