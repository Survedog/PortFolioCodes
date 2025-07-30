// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/CPGameplayAbility.h"
#include "Interface/CPStaminaCostAbilityInterface.h"
#include "GA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGA_Sprint : public UCPGameplayAbility, public ICPStaminaCostAbilityInterface
{
	GENERATED_BODY()

public:
	UGA_Sprint();

	FORCEINLINE const FScalableFloat& GetStaminaCost() const override { return StaminaCost; }

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ApplyCost_SaveCostEffectHandle(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

protected:
	void MoveTagEventCallback(FGameplayTag Tag, int32 NewTagCount);

	FActiveGameplayEffectHandle ActiveEffectHandle;
	FActiveGameplayEffectHandle ActiveCostEffectHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> SprintEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat StaminaCost;
};
