// Copyright 2024, Survedog. All rights reserved.


#include "GA/GA_PrimaryAttack.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "Weapon/Weapon.h"
#include "GameFramework/Character.h"
#include "Charge.h"

UGA_PrimaryAttack::UGA_PrimaryAttack() : InputBufferEnableDelay(1.0f)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_PrimaryAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(ActorInfo->AvatarActor.Get());
	if (!AttackInterface)
	{
		CP_NETGASLOG(LogCPGAS, Error, TEXT("The AvatarActor doesn't implement ICPCharacterAttackInterface."));
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}

	const float ChargeRate = TriggerEventData ? TriggerEventData->EventMagnitude : 0.0f;
	UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
	if (Weapon)
	{
 		Weapon->OnAttackComboProceed.AddDynamic(this, &UGA_PrimaryAttack::StartInputBufferDelayTimer);
		Weapon->OnAttackEnd.AddDynamic(this, &UGA_PrimaryAttack::K2_EndAbility);
		Weapon->StartComboAttack(ChargeRate);
	}
	else
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
}

void UGA_PrimaryAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bIsInputBufferEnabled = false;
	ICPCharacterAttackInterface* AttackInterface = CastChecked<ICPCharacterAttackInterface>(ActorInfo->AvatarActor.Get());
	UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
	if (Weapon)
	{
		Weapon->OnAttackComboProceed.RemoveDynamic(this, &UGA_PrimaryAttack::StartInputBufferDelayTimer);
		Weapon->OnAttackEnd.RemoveDynamic(this, &UGA_PrimaryAttack::K2_EndAbility);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PrimaryAttack::ServerRPCInputPressed_Implementation(FVector_NetQuantize InClientLocation, FRotator InClientRotation, FRotator InClientControlRotation)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACharacter* Character = CastChecked<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	Character->SetActorLocationAndRotation(InClientLocation, InClientRotation);
	Character->GetController()->SetControlRotation(InClientControlRotation);
	InputPressed(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UGA_PrimaryAttack::StartInputBufferDelayTimer()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	GetWorld()->GetTimerManager().SetTimer(InputBufferDelayTimerHandle, this, &UGA_PrimaryAttack::EnableInputBuffer, InputBufferEnableDelay, false);
}

void UGA_PrimaryAttack::EnableInputBuffer()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	InputBufferDelayTimerHandle.Invalidate();
	bIsInputBufferEnabled = true;
}

void UGA_PrimaryAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Do not call super function.
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ICPCharacterAttackInterface* AttackInterface = CastChecked<ICPCharacterAttackInterface>(ActorInfo->AvatarActor.Get());
	if (!HasAuthority(&ActivationInfo))
	{
		ServerRPCInputPressed(AttackInterface->GetAttackerLocation(), AttackInterface->GetAttackerRotation(), AttackInterface->GetAttackerControlRotation());
	}

	UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
	if (Weapon && Weapon->IsAttacking())
	{
		if (Weapon->IsWaitingForNextAttack())
		{
			Weapon->ProceedComboAttack();
		}
		else if (bIsInputBufferEnabled)
		{
			Weapon->SetHasAttackInput(true);
		}
		bIsInputBufferEnabled = false;
	}
	else
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
}
