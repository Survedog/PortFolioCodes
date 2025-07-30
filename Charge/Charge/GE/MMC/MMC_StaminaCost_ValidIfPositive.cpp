// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/MMC/MMC_StaminaCost_ValidIfPositive.h"
#include "Attribute/CPStaminaAttributeSet.h"
#include "Interface/CPStaminaCostAbilityInterface.h"

UMMC_StaminaCost_ValidIfPositive::UMMC_StaminaCost_ValidIfPositive()
{
	StaminaDef.AttributeToCapture = UCPStaminaAttributeSet::GetStaminaAttribute();
	StaminaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	StaminaDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StaminaDef);
}

float UMMC_StaminaCost_ValidIfPositive::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float CurrentStamina = 0.0f;
	GetCapturedAttributeMagnitude(StaminaDef, Spec, EvaluationParameters, CurrentStamina);

	float StaminaCost = -Super::CalculateBaseMagnitude_Implementation(Spec);
	if (StaminaCost > KINDA_SMALL_NUMBER)
	{
		StaminaCost = FMath::Clamp(CurrentStamina, KINDA_SMALL_NUMBER, StaminaCost);
	}
	else
	{
		StaminaCost = 0.0f;
	}

	return -StaminaCost;
}
