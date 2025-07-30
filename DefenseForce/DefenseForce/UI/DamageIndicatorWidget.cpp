// Copyright 2024, Survedog. All rights reserved.


#include "UI/DamageIndicatorWidget.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attribute/DFHealthAttributeSet.h"

UAbilitySystemComponent* UDamageIndicatorWidget::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

void UDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASC.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UDFHealthAttributeSet::GetHpAttribute()).AddUObject(this, &UDamageIndicatorWidget::OnOwnerHpChangedCallback);
	}
}

void UDamageIndicatorWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ASC.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UDFHealthAttributeSet::GetHpAttribute()).RemoveAll(this);
	}
}

void UDamageIndicatorWidget::OnOwnerHpChangedCallback(const FOnAttributeChangeData& AttributeChangeData)
{
	const float DamageAmount = AttributeChangeData.OldValue - AttributeChangeData.NewValue;
	if (DamageAmount >= 0.0f)
	{
		ShowDamageAmount(DamageAmount);
	}
}

void UDamageIndicatorWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}
