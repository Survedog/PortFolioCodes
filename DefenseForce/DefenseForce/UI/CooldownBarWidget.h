// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interface/DFGASUserWidgetInterface.h"
#include "AbilitySystemInterface.h"
#include "DFGaugeWidgetBase.h"
#include "CooldownBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UCooldownBarWidget : public UDFGaugeWidgetBase, public IDFGASUserWidgetInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void SetAbilitySystemComponent(class UAbilitySystemComponent* NewASC) override;

	void OnActiveGameplayEffectAddedCallback(class UAbilitySystemComponent* InASC, const struct FGameplayEffectSpec& InEffectSpec, FActiveGameplayEffectHandle InActiveEffectHandle);
	void OnCooldownTagChanged(const FGameplayTag Tag, int32 TagCount);

protected:
	const struct FActiveGameplayEffect* ActiveCooldownEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASC")
	TWeakObjectPtr<class UAbilitySystemComponent> ASC;
};
