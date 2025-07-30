// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/DFGameplayAbility.h"
#include "GA_BuildStructure.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UGA_BuildStructure : public UDFGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_BuildStructure();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;	

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(Server, Reliable)
	void ServerRPCSpawnTargetStructure(TSubclassOf<class ADFStructureBase> InTargetStructureClass, const FVector_NetQuantize SpawnLocation);

	UFUNCTION()
	void OnRep_BuiltStructure();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Build")
	TSubclassOf<class ADFStructureBase> TargetStructureClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_BuiltStructure, Category = "Build")
	TWeakObjectPtr<class ADFStructureBase> BuiltStructure;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TSubclassOf<class ADFGATA_ActorPlacement> ActorPlacementTAClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Targeting")
	TObjectPtr<class ADFGATA_ActorPlacement> DFActorPlacementTA;	

	/** Which reticle class to use for targeting. Overrides reticle class field of target actor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TSubclassOf<class AGAWorldReticle_ActorVisualization> ActorVisualReticleClass;
};
