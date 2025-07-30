// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GASUserWidget.h"
#include "GameplayEffectTypes.h"
#include "AttributeBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UAttributeBarUserWidget : public UGASUserWidget
{
	GENERATED_BODY()

protected:
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* NewASC) override;

	void OnAttributeValueChanged(const FOnAttributeChangeData& ChangeData);
	void OnAttributeMaxValueChanged(const FOnAttributeChangeData& ChangeData);

	UFUNCTION(BlueprintCallable)
	void UpdateAttributeBar();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtAttributeValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbAttributeBar;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute TargetMaxAttribute;

	UPROPERTY(EditAnywhere)
	uint8 bShowValueText : 1 = true;

	UPROPERTY()
	float CurrentAttributeValue = 0.0f;

	UPROPERTY()
	float CurrentAttributeMaxValue = 0.1f;
};
