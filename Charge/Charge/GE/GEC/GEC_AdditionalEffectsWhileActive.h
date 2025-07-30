// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "GEC_AdditionalEffectsWhileActive.generated.h"

/** Add additional Gameplay Effects that attempt to activate under certain conditions (or no conditions).
 * Additional Gameplay Effects that are applied on application are removed when the owner GE is removed.
 * Read AdditionalEffectsGameplayEffectComponent.h for more details. */
UCLASS(CollapseCategories, DisplayName="AdditionalEffectsWhileActive")
class CHARGE_API UGEC_AdditionalEffectsWhileActive : public UGameplayEffectComponent
{
	GENERATED_BODY()
	
public:
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& GEContainer, FActiveGameplayEffect& ActiveGE) const override;
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

protected:
	void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const;

public:
	/**
	 * This will copy all of the data (e.g. SetByCallerMagnitudes) from the GESpec that Applied this GameplayEffect to the new OnApplicationGameplayEffect Specs.
	 * One would think this is normally desirable (and how OnComplete works by default), but we default to false here for backwards compatability.
	 */
	UPROPERTY(EditDefaultsOnly)
	bool bCopyDataFromOriginalSpec = false;

	/** Other gameplay effects that will be applied to the target of this effect if the owning effect applies */
	UPROPERTY(EditDefaultsOnly)
	TArray<FConditionalGameplayEffect> WhileActiveGameplayEffects;

protected:
	mutable TArray<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles;
};
