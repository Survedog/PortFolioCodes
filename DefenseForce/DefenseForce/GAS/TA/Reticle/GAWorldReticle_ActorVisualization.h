// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/TA/Reticle/DFGAWorldReticle.h"
#include "GAWorldReticle_ActorVisualization.generated.h"

/**
 * Custom class of AGameplayAbilityWorldReticle_ActorVisualization.
 */
UCLASS(NotPlaceable)
class DEFENSEFORCE_API AGAWorldReticle_ActorVisualization : public ADFGAWorldReticle
{
	GENERATED_BODY()

public:
	AGAWorldReticle_ActorVisualization(const FObjectInitializer& ObjectInitializer);

	void InitializeVisualizationInformation(TSubclassOf<AActor> InPlacedActorClass, UMaterialInterface* InVisualizationMaterial);

	/** Returns CollisionComponent subobject **/
	class UCapsuleComponent* GetCollisionComponent() const { return CollisionComponent; }

protected:
	void AttachMeshComponents(TSubclassOf<AActor> InPlacedActorClass, UMaterialInterface* InVisualizationMaterial);
	void DestroyMeshComponents();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	TSubclassOf<AActor> CurrentPlacedActorClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	TObjectPtr<UMaterialInterface> CurrentVisualizationMaterial;

	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	TArray<TObjectPtr<UMeshComponent>> MeshComps;

private:
	/** Hardcoded collision component, so other objects don't think they can collide with the visualization actor */
	UPROPERTY()
	TObjectPtr<class UCapsuleComponent> CollisionComponent;
};
