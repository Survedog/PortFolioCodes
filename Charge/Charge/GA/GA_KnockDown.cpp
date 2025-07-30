// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_KnockDown.h"
#include "Character/CPCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GASTag/CPGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Components/CapsuleComponent.h"
#include "Physics/CPCollision.h"
#include "Charge.h"

UGA_KnockDown::UGA_KnockDown()
{
	ActivationBlockedTags.RemoveTag(GASTAG_CHARACTER_STATE_IsStunned);
	ActivationBlockedTags.RemoveTag(GASTAG_CHARACTER_STATE_IsKnockedDown);

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	static ConstructorHelpers::FClassFinder<UGameplayEffect> InvincibleEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_Invincible_Evasive_Infinite.BPGE_Invincible_Evasive_Infinite_C"));
	if (InvincibleEffectClassRef.Class)
	{
		InvincibleEffectClass = InvincibleEffectClassRef.Class;
	}
}

void UGA_KnockDown::EnableGettingUp()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));

	APawn* AvatarPawn = CastChecked<APawn>(CurrentActorInfo->AvatarActor);
	if (AvatarPawn->IsPlayerControlled())
	{
		UAbilityTask_WaitGameplayTagAdded* WaitMoveTagAddedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, GASTAG_CHARACTER_ACTION_Move, nullptr, true);
		WaitMoveTagAddedTask->Added.AddDynamic(this, &UGA_KnockDown::OnMoveTagAddedCallback);
		WaitMoveTagAddedTask->ReadyForActivation();
	}
	else
	{
		OnMoveTagAddedCallback();
	}
}

void UGA_KnockDown::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ACPCharacterBase* CPCharacter = CastChecked<ACPCharacterBase>(ActorInfo->AvatarActor);
	const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult();
	CPCharacter->SetActorRotation(HitResult->ImpactNormal.GetSafeNormal2D().ToOrientationQuat());
	CPCharacter->GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_BLOCKONLYWORLD);
	CPCharacter->GetMesh()->SetCollisionProfileName(CPROFILE_BLOCKONLYWORLD);

	UAbilityTask_PlayMontageAndWait* PlayKnockDownMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayKnockDownMontage"), CPCharacter->GetKnockedDownMontage());
	// TODO: Handle the case when the montage is interrupted.
	 //PlayKnockDownMontageTask->OnInterrupted.AddDynamic(this, &UGA_KnockDown::K2_CancelAbility);
	PlayKnockDownMontageTask->ReadyForActivation();

	CPCharacter->SetIgnoreMovement(true);
	
	if (CPCharacter->HasAuthority())
	{
		ActiveInvincibleEffectHandle = BP_ApplyGameplayEffectToOwner(InvincibleEffectClass);
	}	
}

void UGA_KnockDown::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));

	ACPCharacterBase* CPCharacter = CastChecked<ACPCharacterBase>(ActorInfo->AvatarActor);
	CPCharacter->SetIgnoreMovement(false);
	CPCharacter->GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CPCharacter->GetMesh()->SetCollisionProfileName(CPROFILE_CHARACTERMESH);
	
	if (ActiveInvincibleEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveInvincibleEffectHandle);
		ActiveInvincibleEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_KnockDown::OnMoveTagAddedCallback()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACPCharacterBase* CPCharacter = CastChecked<ACPCharacterBase>(CurrentActorInfo->AvatarActor);
	UAbilityTask_PlayMontageAndWait* PlayKnockDownMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayKnockDownMontage"), CPCharacter->GetKnockedDownMontage(), 1.0f, TEXT("GetUp"));
	//PlayKnockDownMontageTask->OnInterrupted.AddDynamic(this, &UGA_KnockDown::K2_EndAbility);
	PlayKnockDownMontageTask->OnCompleted.AddDynamic(this, &UGA_KnockDown::K2_EndAbility);
	PlayKnockDownMontageTask->ReadyForActivation();
}
