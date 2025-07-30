// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/TA/Reticle/DFGAWorldReticle.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "GAWorldReticle_ProjectilePath.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API AGAWorldReticle_ProjectilePath : public ADFGAWorldReticle
{
	GENERATED_BODY()
	
public:
	AGAWorldReticle_ProjectilePath(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPredictPathResultSet(const FPredictProjectilePathResult& PredictPathResult);
};
