// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_StaminaCost.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UMMC_StaminaCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
