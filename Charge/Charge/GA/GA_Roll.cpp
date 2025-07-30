// Copyright 2024, Survedog. All rights reserved.


#include "GA/GA_Roll.h"
#include "Character/CPPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "GASTag/CPGameplayTag.h"
#include "AbilitySystemGlobals.h"
#include "Charge.h"

UGA_Roll::UGA_Roll() : StaminaCost(10.f), Cooldown(0.7f), InvincibleTime(0.3f)
{
	bServerRespectsRemoteAbilityCancellation = true;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	static ConstructorHelpers::FClassFinder<UGameplayEffect> InvincibleEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_Invincible_Evasive.BPGE_Invincible_Evasive_C"));
	if (InvincibleEffectClassRef.Class)
	{
		InvincibleEffect = InvincibleEffectClassRef.Class;
	}
}

bool UGA_Roll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
	return !AvatarCPCharacter->GetCharacterMovement()->IsFalling();
}

void UGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (IsLocallyControlled())
	{
		ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());

		FVector PendingMovementInputVector = AvatarCPCharacter->GetPendingMovementInputVector();
		FRotator RollRotation = FRotator::ZeroRotator;
		if (!PendingMovementInputVector.IsNearlyZero())
		{
			RollRotation = FRotationMatrix::MakeFromX(PendingMovementInputVector).Rotator();
			AvatarCPCharacter->SetActorRotation(RollRotation);
		}
		UAbilityTask_PlayMontageAndWait* PlayRollMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayRollMontage"), AvatarCPCharacter->GetRollMontage());
		PlayRollMontageTask->OnBlendOut.AddDynamic(this, &UGA_Roll::OnCompletedCallback);
		PlayRollMontageTask->OnInterrupted.AddDynamic(this, &UGA_Roll::OnInterruptedCallback);
		PlayRollMontageTask->ReadyForActivation();

		if (RollRotation.IsNearlyZero())
		{
			AvatarCPCharacter->ServerRPCPlayMontageOnRemotes(AvatarCPCharacter->GetRollMontage());
		}
		else
		{
			AvatarCPCharacter->ServerRPCPlayMontageOnRemotes_AdjustRotation(AvatarCPCharacter->GetRollMontage(), RollRotation);
		}
	}

	FGameplayEffectSpecHandle InvincibleEffectSpecHandle = MakeOutgoingGameplayEffectSpec(InvincibleEffect);
	InvincibleEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_CHARACTER_STATE_Invincible, InvincibleTime);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, InvincibleEffectSpecHandle);
}

void UGA_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Roll::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	// Don't call super function.
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass());
		EffectSpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(FGameplayTagContainer(GASTAG_CHARACTER_COOLDOWN_Roll));
		EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_CHARACTER_COOLDOWN, Cooldown);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
	}
}

void UGA_Roll::OnCompletedCallback()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Roll::OnInterruptedCallback()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
