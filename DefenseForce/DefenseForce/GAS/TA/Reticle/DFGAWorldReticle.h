// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityWorldReticle.h"
#include "DFGAWorldReticle.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFGAWorldReticle : public AGameplayAbilityWorldReticle
{
	GENERATED_BODY()
	
public: 
	ADFGAWorldReticle(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent)
	void OnTraceResultSet(const FHitResult& HitResult);

protected:
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Should it keep rotating to face owner? Tick function must be enabled to use this functionality. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	uint8 bShouldFaceOwner : 1;
};
