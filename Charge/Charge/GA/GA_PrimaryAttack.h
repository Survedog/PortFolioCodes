// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "GA_PrimaryAttack.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_PrimaryAttack : public UCPGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_PrimaryAttack();

protected:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(Server, Reliable)
	void ServerRPCInputPressed(FVector_NetQuantize InClientLocation, FRotator InClientRotation, FRotator InClientControlRotation);

	UFUNCTION()
	FORCEINLINE void StartInputBufferDelayTimer();

	FORCEINLINE void EnableInputBuffer();

protected:
	UPROPERTY(EditAnywhere)
	float InputBufferEnableDelay;

	FTimerHandle InputBufferDelayTimerHandle;

	uint8 bIsInputBufferEnabled : 1;
};
