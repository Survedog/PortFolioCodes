// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Aim.h"
#include "Character/CPPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/Data/CPCharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/CPHUDWidget.h"
#include "Charge.h"

UGA_Aim::UGA_Aim()
{
	bServerRespectsRemoteAbilityCancellation = true;
}

void UGA_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
	const UCPCharacterControlData* AimingControlData = AvatarCPCharacter->GetAimingModeControlData();

	USpringArmComponent* CameraBoom = AvatarCPCharacter->GetCameraBoom();
	CameraBoom->TargetArmLength = AimingControlData->TargetArmLength;
	CameraBoom->SocketOffset = AimingControlData->SocketOffset;
	CameraBoom->bDoCollisionTest = AimingControlData->bDoCollisionTest;

	if (IsLocallyControlled())
	{
		AvatarCPCharacter->SetCrosshairVisibility(true);
	}

	AvatarCPCharacter->bUseControllerRotationYaw = AimingControlData->bUseControllerRotationYaw;
	AvatarCPCharacter->GetCharacterMovement()->bOrientRotationToMovement = AimingControlData->bOrientRotationToMovement;
	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void UGA_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
	const UCPCharacterControlData* IdleControlData = AvatarCPCharacter->GetIdleModeControlData();

	if (IsLocallyControlled())
	{
		USpringArmComponent* CameraBoom = AvatarCPCharacter->GetCameraBoom();
		CameraBoom->TargetArmLength = IdleControlData->TargetArmLength;
		CameraBoom->SocketOffset = IdleControlData->SocketOffset;
		CameraBoom->bDoCollisionTest = IdleControlData->bDoCollisionTest;

		AvatarCPCharacter->SetCrosshairVisibility(false);
	}

	AvatarCPCharacter->bUseControllerRotationYaw = IdleControlData->bUseControllerRotationYaw;
	AvatarCPCharacter->GetCharacterMovement()->bOrientRotationToMovement = IdleControlData->bOrientRotationToMovement;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Do not call Super function.
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}