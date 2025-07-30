// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GE/MMC/MMC_StaminaCost.h"
#include "MMC_StaminaCost_ValidIfPositive.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UMMC_StaminaCost_ValidIfPositive : public UMMC_StaminaCost
{
	GENERATED_BODY()

public:
	UMMC_StaminaCost_ValidIfPositive();

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition StaminaDef;
};
