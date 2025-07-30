// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "CPParamGameplayAbility.generated.h"

/**
 * 
 */
// For gameplay abilities that need input action value for their job.
UCLASS()
class CHARGE_API UCPParamGameplayAbility : public UCPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCPParamGameplayAbility();

	UFUNCTION()
	virtual void TryInputActionPressed(const FInputActionValue& Value);
	
protected:
	// Sets BindAbilityToMatchingInputAction function to be called when player character binds input actions.
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// If a child class needs input action value for its job, it can be done here. (Not ActivateAbility)
	// Only called in local. EndAbility must be called in either one of these. (InputPressed/InputReleased)

	UFUNCTION()
	virtual void InputActionPressed(const FInputActionValue& Value) {};

	UFUNCTION()
	virtual void InputActionReleased(const FInputActionValue& Value) {};

private:
	void BindAbilityToMatchingInputAction();

protected:
	UPROPERTY()
	uint8 bBindInputPressed : 1 = true;

	UPROPERTY()
	uint8 bBindInputReleased : 1 = true;
};
