// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DFGaugeWidgetBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DEFENSEFORCE_API UDFGaugeWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void UpdateGaugeCurrentValue(float InCurrentValue) { GaugeCurrentValue = InCurrentValue; UpdateGaugeUI(GaugeCurrentValue, GaugeMaxValue); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void UpdateGaugeMaxValue(float InMaxValue) { GaugeMaxValue = InMaxValue; UpdateGaugeUI(GaugeCurrentValue, GaugeMaxValue); }

	FORCEINLINE void UpdateGaugeCurrentAndMaxValue(float InCurrentValue, float InMaxValue) { GaugeCurrentValue = InCurrentValue; GaugeMaxValue = InMaxValue; UpdateGaugeUI(GaugeCurrentValue, GaugeMaxValue); }

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateGaugeUI(float InCurrentValue, float InMaxValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetGaugeCurrentValue() const { return GaugeCurrentValue; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetGaugeMaxValue() const { return GaugeMaxValue; }

private:
	UPROPERTY(VisibleAnywhere)
	float GaugeCurrentValue;

	UPROPERTY(VisibleAnywhere)
	float GaugeMaxValue;
};
