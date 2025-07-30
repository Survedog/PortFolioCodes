// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CPChargeAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class CHARGE_API UCPChargeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCPChargeAttributeSet();

	ATTRIBUTE_ACCESSORS(UCPChargeAttributeSet, ChargeAmount);
	ATTRIBUTE_ACCESSORS(UCPChargeAttributeSet, MaxChargeAmount);
	ATTRIBUTE_ACCESSORS(UCPChargeAttributeSet, ChargeSpeed);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ChargeAmount, BlueprintReadOnly, Category = "Charge", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChargeAmount;

	UPROPERTY(ReplicatedUsing = OnRep_MaxChargeAmount, BlueprintReadOnly, Category = "Charge", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxChargeAmount;

	UPROPERTY(BlueprintReadOnly, Category = "Charge", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChargeSpeed;

protected:
	UFUNCTION()
	void OnRep_ChargeAmount(const FGameplayAttributeData& OldChargeAmount);

	UFUNCTION()
	void OnRep_MaxChargeAmount(const FGameplayAttributeData& OldMaxChargeAmount);
};
