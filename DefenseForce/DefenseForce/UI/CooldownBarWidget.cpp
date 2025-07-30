// Copyright 2024, Survedog. All rights reserved.


#include "UI/CooldownBarWidget.h"
#include "AbilitySystemComponent.h"
#include "GAS/DFGameplayTags.h"
#include "Components/ProgressBar.h"
#include "DFLog.h"

void UCooldownBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	DF_NETUILOG(LogDFUI, Log, TEXT("Start"));
	if (ASC.IsValid())
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UCooldownBarWidget::OnActiveGameplayEffectAddedCallback);
		ASC->RegisterGameplayTagEvent(GASTAG_Structure_Action_Attack_Cooldown, EGameplayTagEventType::Type::NewOrRemoved).AddUObject(this, &UCooldownBarWidget::OnCooldownTagChanged);
	}	
	SetVisibility(ESlateVisibility::Hidden);
}

void UCooldownBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ActiveCooldownEffect)
	{		
		const float CooldownElaspedTime = ActiveCooldownEffect->GetDuration() - ActiveCooldownEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		UpdateGaugeCurrentValue(CooldownElaspedTime);
	}
}

void UCooldownBarWidget::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& InEffectSpec, FActiveGameplayEffectHandle InActiveEffectHandle)
{
	DF_NETUILOG(LogDFUI, Log, TEXT("Start"));

	FGameplayTagContainer EffectAssetTagContainer;
	InEffectSpec.GetAllAssetTags(EffectAssetTagContainer);
	if (EffectAssetTagContainer.HasTag(GASTAG_Structure_Action_Attack_Cooldown))
	{
		ActiveCooldownEffect = InASC->GetActiveGameplayEffect(InActiveEffectHandle);
		if (ActiveCooldownEffect)
		{
			UpdateGaugeCurrentAndMaxValue(0.0f, ActiveCooldownEffect->GetDuration());
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UCooldownBarWidget::OnCooldownTagChanged(const FGameplayTag Tag, int32 TagCount)
{
	DF_NETUILOG(LogDFUI, Log, TEXT("Start. TagCount: %d"), TagCount);
	if (TagCount <= 0)
	{
		SetVisibility(ESlateVisibility::Hidden);
		ActiveCooldownEffect = nullptr;
	}
}

void UCooldownBarWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}

UAbilitySystemComponent* UCooldownBarWidget::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
