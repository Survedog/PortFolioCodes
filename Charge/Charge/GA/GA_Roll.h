// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "Interface/CPStaminaCostAbilityInterface.h"
#include "GA_Roll.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_Roll : public UCPGameplayAbility, public ICPStaminaCostAbilityInterface
{
	GENERATED_BODY()
	
public:
	UGA_Roll();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FORCEINLINE const FScalableFloat& GetStaminaCost() const override { return StaminaCost; }
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	UFUNCTION()
	void OnCompletedCallback();

	UFUNCTION()
	void OnInterruptedCallback();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InvincibleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InvincibleTime;
};
