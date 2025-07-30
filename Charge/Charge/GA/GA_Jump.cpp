// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Jump.h"
#include "GameFramework/Character.h"
#include "Charge.h"

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor);
	bCanActivate = bCanActivate && Character->CanJump();
	return bCanActivate;
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor);
	Character->LandedDelegate.AddDynamic(this, &UGA_Jump::OnLandedCallback);
	Character->Jump();
}

void UGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor);
	Character->LandedDelegate.RemoveDynamic(this, &UGA_Jump::OnLandedCallback);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	// Do not call Super function.
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor);
	Character->StopJumping();
}

void UGA_Jump::OnLandedCallback(const FHitResult& Hit)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
