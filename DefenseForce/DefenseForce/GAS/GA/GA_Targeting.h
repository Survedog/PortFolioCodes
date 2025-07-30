// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/DFGameplayAbility.h"
#include "GA_Targeting.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UGA_Targeting : public UDFGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<class AGameplayAbilityTargetActor> DFTargetActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Control")
	TObjectPtr<class ADFTowerBase> ControlledTower;
};
