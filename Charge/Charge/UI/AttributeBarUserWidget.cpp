// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AttributeBarUserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "Charge.h"

void UAttributeBarUserWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
	Super::SetAbilitySystemComponent(NewASC);

	const ESlateVisibility TextVisibility = bShowValueText? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	TxtAttributeValue->SetVisibility(TextVisibility);

	bool bIsAttributeFound, bIsMaxAttributeFound;
	CurrentAttributeValue = NewASC->GetGameplayAttributeValue(TargetAttribute, bIsAttributeFound);
	CurrentAttributeMaxValue = NewASC->GetGameplayAttributeValue(TargetMaxAttribute, bIsMaxAttributeFound);

	if (!bIsAttributeFound || !bIsMaxAttributeFound)
	{
		CP_LOG(LogCPGAS, Error, TEXT("Owner ASC doens't have target attribute."));
		return;
	}
	else if (CurrentAttributeMaxValue > 0.0f)
	{
		UpdateAttributeBar();
	}

	NewASC->GetGameplayAttributeValueChangeDelegate(TargetAttribute).AddUObject(this, &UAttributeBarUserWidget::OnAttributeValueChanged);
	NewASC->GetGameplayAttributeValueChangeDelegate(TargetMaxAttribute).AddUObject(this, &UAttributeBarUserWidget::OnAttributeMaxValueChanged);
}

void UAttributeBarUserWidget::OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeValue = ChangeData.NewValue;
	UpdateAttributeBar();
}

void UAttributeBarUserWidget::OnAttributeMaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeMaxValue = ChangeData.NewValue;
	UpdateAttributeBar();
}

void UAttributeBarUserWidget::UpdateAttributeBar()
{
	if (TxtAttributeValue && bShowValueText)
	{
		TxtAttributeValue->SetText(FText::FromString(FString::Printf(TEXT("%.1f/%.1f"), CurrentAttributeValue, CurrentAttributeMaxValue)));
	}

	if (PbAttributeBar)
	{
		PbAttributeBar->SetPercent(CurrentAttributeValue / CurrentAttributeMaxValue);
	}
}
