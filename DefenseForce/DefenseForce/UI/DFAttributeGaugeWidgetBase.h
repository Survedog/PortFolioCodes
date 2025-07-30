// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/DFGaugeWidgetBase.h"
#include "AbilitySystemInterface.h" 
#include "AttributeSet.h"
#include "Interface/DFGASUserWidgetInterface.h"

#include "DFAttributeGaugeWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UDFAttributeGaugeWidgetBase : public UDFGaugeWidgetBase, public IDFGASUserWidgetInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void SetAbilitySystemComponent(class UAbilitySystemComponent* NewASC) override;

	void OnAttributeValueChanged(const struct FOnAttributeChangeData& ChangeData);
	void OnAttributeMaxValueChanged(const struct FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASC")
	TWeakObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute TargetMaxAttribute;
};
