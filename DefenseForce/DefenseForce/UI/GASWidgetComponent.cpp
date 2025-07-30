// Copyright 2024, Survedog. All rights reserved.


#include "UI/GASWidgetComponent.h"
#include "Interface/DFGASUserWidgetInterface.h"
#include "AbilitySystemBlueprintLibrary.h"

void UGASWidgetComponent::InitWidget()
{
	Super::InitWidget();

	IDFGASUserWidgetInterface* GASWidgetInterface = Cast<IDFGASUserWidgetInterface>(GetWidget());
	if (GASWidgetInterface)
	{
		GASWidgetInterface->SetAbilitySystemComponent(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	}
}
