// Copyright 2024, Survedog. All rights reserved.


#include "UI/DFAttributeGaugeWidgetBase.h"
#include "AbilitySystemComponent.h"
#include "DFLog.h"

UAbilitySystemComponent* UDFAttributeGaugeWidgetBase::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

void UDFAttributeGaugeWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASC.IsValid())
	{
		bool bIsAttributeFound, bIsMaxAttributeFound;
		const float CurrentAttributeValue = ASC->GetGameplayAttributeValue(TargetAttribute, bIsAttributeFound);
		const float CurrentAttributeMaxValue = ASC->GetGameplayAttributeValue(TargetMaxAttribute, bIsMaxAttributeFound);

		if (!bIsAttributeFound || !bIsMaxAttributeFound)
		{
			DF_LOG(LogDFGAS, Error, TEXT("Owner ASC doens't have target attribute."));
			return;
		}

		UpdateGaugeCurrentAndMaxValue(CurrentAttributeValue, CurrentAttributeMaxValue);
		ASC->GetGameplayAttributeValueChangeDelegate(TargetAttribute).AddUObject(this, &UDFAttributeGaugeWidgetBase::OnAttributeValueChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(TargetMaxAttribute).AddUObject(this, &UDFAttributeGaugeWidgetBase::OnAttributeMaxValueChanged);
	}
}

void UDFAttributeGaugeWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

	if (ASC.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(TargetAttribute).RemoveAll(this);
		ASC->GetGameplayAttributeValueChangeDelegate(TargetMaxAttribute).RemoveAll(this);
	}
}

void UDFAttributeGaugeWidgetBase::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}

void UDFAttributeGaugeWidgetBase::OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData)
{
	UpdateGaugeCurrentValue(ChangeData.NewValue);
}

void UDFAttributeGaugeWidgetBase::OnAttributeMaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	UpdateGaugeMaxValue(ChangeData.NewValue);
}
