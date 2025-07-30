// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CPCharacterAnimInstance.h"
#include "Character/CPCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASTag/CPGameplayTag.h"
#include "Charge.h"

UCPCharacterAnimInstance::UCPCharacterAnimInstance() : CPCharacter(nullptr), MovementComp(nullptr)
{
	GroundSpeed = 0.0f;
	MovementYawRotation = 0.0f;
	bIsFalling = false;
	bIsDead = false;
	bIsJumping = false;
	JumpThreshold = 100.0f;
}

void UCPCharacterAnimInstance::NativeInitializeAnimation()
{
	CP_NETCUSTOMLOG(GetOwningActor(), LogCP, Log, TEXT("Start"));
	Super::NativeInitializeAnimation();

	CPCharacter = Cast<ACPCharacterBase>(GetOwningActor());
	if (CPCharacter.IsValid())
	{
		MovementComp = CPCharacter->GetCharacterMovement();
	}
}

void UCPCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComp.IsValid())
	{
		GroundSpeed = MovementComp->Velocity.Size2D();
		MovementYawRotation = UKismetAnimationLibrary::CalculateDirection(MovementComp->Velocity, CPCharacter->GetControlRotation());
		bIsFalling = MovementComp->IsFalling();
		bIsJumping = bIsFalling & (MovementComp->Velocity.Z > JumpThreshold);
	}
}

void UCPCharacterAnimInstance::SetCharacterASC(UAbilitySystemComponent* InASC)
{
	CP_NETCUSTOMLOG(GetOwningActor(), LogCP, Log, TEXT("Start"));

	CharacterASC = InASC;
	if (CharacterASC)
	{
		CharacterASC->RegisterGameplayTagEvent(GASTAG_CHARACTER_STATE_IsDead).AddUObject(this, &UCPCharacterAnimInstance::DeadTagChangeEventCallback);
	}
}

UAbilitySystemComponent* UCPCharacterAnimInstance::GetAbilitySystemComponent() const
{
	return CharacterASC;
}

void UCPCharacterAnimInstance::DeadTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount)
{
	bIsDead = NewTagCount > 0;
}
