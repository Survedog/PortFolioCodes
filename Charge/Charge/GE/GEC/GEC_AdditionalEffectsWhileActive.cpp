// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GEC/GEC_AdditionalEffectsWhileActive.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "UGEC_AdditionalEffectsWhileActive"

bool UGEC_AdditionalEffectsWhileActive::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	// We don't allow prediction of expiration (on removed) effects
	if (ActiveGEContainer.IsNetAuthority())
	{
		// When this ActiveGE gets removed, so will our events so no need to unbind this.
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &UGEC_AdditionalEffectsWhileActive::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
	}

	return true;
}

void UGEC_AdditionalEffectsWhileActive::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	if (!ensureMsgf(ActiveGEContainer.Owner, TEXT("OnGameplayEffectApplied is passed an ActiveGEContainer which lives within an ASC but that ASC was somehow null")))
	{
		return;
	}

	const float GELevel = GESpec.GetLevel();
	const FGameplayEffectContextHandle& GEContextHandle = GESpec.GetEffectContext();

	/** other effects that need to be applied to the target if this effect is successful */
	TArray<FGameplayEffectSpecHandle> TargetEffectSpecs;
	for (const FConditionalGameplayEffect& ConditionalEffect : WhileActiveGameplayEffects)
	{
		const UGameplayEffect* GameplayEffectDef = ConditionalEffect.EffectClass.GetDefaultObject();
		if (!GameplayEffectDef)
		{
			continue;
		}

		if (ConditionalEffect.CanApply(GESpec.CapturedSourceTags.GetActorTags(), GELevel))
		{
			FGameplayEffectSpecHandle SpecHandle;
			if (bCopyDataFromOriginalSpec)
			{
				SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec());
				SpecHandle.Data->InitializeFromLinkedSpec(GameplayEffectDef, GESpec);
			}
			else
			{
				SpecHandle = ConditionalEffect.CreateSpec(GEContextHandle, GELevel);
			}

			if (ensure(SpecHandle.IsValid()))
			{
				TargetEffectSpecs.Add(SpecHandle);
			}
		}
	}

	// Add all of the dynamically linked ones that don't have any conditions applied to them
	// Note: I believe this functionality should be removed as its usage is dynamic and thus not 
	// synchronized between client/server.  I've deprecated the variable to help enforce this.
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		TargetEffectSpecs.Append(GESpec.TargetEffectSpecs);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

		UAbilitySystemComponent& AppliedToASC = *ActiveGEContainer.Owner;
	for (const FGameplayEffectSpecHandle& TargetSpec : TargetEffectSpecs)
	{
		if (TargetSpec.IsValid())
		{
			FActiveGameplayEffectHandle ActiveEffectHandle = AppliedToASC.ApplyGameplayEffectSpecToSelf(*TargetSpec.Data.Get(), PredictionKey);
			ActiveGameplayEffectHandles.Add(ActiveEffectHandle);
		}
	}
}

void UGEC_AdditionalEffectsWhileActive::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	FScopedActiveGameplayEffectLock ActiveScopeLock(*ActiveGEContainer);

	const FActiveGameplayEffect* ActiveGE = RemovalInfo.ActiveEffect;
	if (!ensure(ActiveGE))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("FGameplayEffectRemovalInfo::ActiveEffect was not populated in OnActiveGameplayEffectRemoved"));
		return;
	}

	UAbilitySystemComponent* ASC = ActiveGEContainer->Owner;
	if (!ensure(IsValid(ASC)))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("ActiveGEContainer was invalid in OnActiveGameplayEffectRemoved"));
		return;
	}

	for (const FActiveGameplayEffectHandle& ActiveEffectHandle : ActiveGameplayEffectHandles)
	{
		if (ActiveEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
		}
	}
	ActiveGameplayEffectHandles.Reset();
}

#if WITH_EDITOR
EDataValidationResult UGEC_AdditionalEffectsWhileActive::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->Period.Value > 0.0f)
	{
		if (WhileActiveGameplayEffects.Num() > 0)
		{
			Context.AddWarning(LOCTEXT("IsPeriodicAndHasOnApplication", "Periodic GE has OnApplicationGameplayEffects. Those GE's will only be applied once."));
		}
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
