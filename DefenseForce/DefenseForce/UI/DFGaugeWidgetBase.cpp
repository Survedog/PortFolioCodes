// Copyright 2024, Survedog. All rights reserved.


#include "UI/DFGaugeWidgetBase.h"

void UDFGaugeWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GaugeCurrentValue = 0.0f;
	GaugeMaxValue = 1.0f;
}
