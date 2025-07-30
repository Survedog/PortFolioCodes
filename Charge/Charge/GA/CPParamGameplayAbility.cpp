// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/CPParamGameplayAbility.h"
#include "Character/CPPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Charge.h"

UCPParamGameplayAbility::UCPParamGameplayAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	bActivateOnGranted = true;
}

void UCPParamGameplayAbility::TryInputActionPressed(const FInputActionValue& Value)
{
	if (CanActivateAbility(CurrentSpecHandle, CurrentActorInfo))
	{
		InputActionPressed(Value);
	}
}

void UCPParamGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ACPPlayerCharacter* CPPlayerCharacter = Cast<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (CPPlayerCharacter)
	{
		CPPlayerCharacter->OnBindingInput.AddUObject(this, &UCPParamGameplayAbility::BindAbilityToMatchingInputAction);
	}
}

void UCPParamGameplayAbility::BindAbilityToMatchingInputAction()
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACPPlayerCharacter* CPPlayerCharacter = Cast<ACPPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (CPPlayerCharacter)
	{
		const TMap<TSubclassOf<UGameplayAbility>, TObjectPtr<UInputAction>> AbilityInputActionMap = CPPlayerCharacter->GetParamAbilityInputMap();
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(CPPlayerCharacter->InputComponent);
		if (EnhancedInputComponent && AbilityInputActionMap.Find(GetClass()))
		{
			UInputAction* SwapWeaponInputAction = AbilityInputActionMap[GetClass()];
			if (bBindInputPressed)
			{
				EnhancedInputComponent->BindAction(SwapWeaponInputAction, ETriggerEvent::Triggered, this, &UCPParamGameplayAbility::TryInputActionPressed);
			}
			if (bBindInputReleased)
			{
				EnhancedInputComponent->BindAction(SwapWeaponInputAction, ETriggerEvent::Completed, this, &UCPParamGameplayAbility::InputActionReleased);
			}
		}
	}
}