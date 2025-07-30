// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/DFGASUserWidgetInterface.h"
#include "AbilitySystemInterface.h"
#include "DamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UDamageIndicatorWidget : public UUserWidget, public IDFGASUserWidgetInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnOwnerHpChangedCallback(const struct FOnAttributeChangeData& AttributeChangeData);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageAmount(float InDamageAmount);

	virtual void SetAbilitySystemComponent(class UAbilitySystemComponent* NewASC) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASC")
	TWeakObjectPtr<class UAbilitySystemComponent> ASC;
};
