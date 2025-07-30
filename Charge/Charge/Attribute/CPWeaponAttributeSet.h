// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CPWeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class CHARGE_API UCPWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCPWeaponAttributeSet();

	ATTRIBUTE_ACCESSORS(UCPWeaponAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UCPWeaponAttributeSet, StunValue);
	ATTRIBUTE_ACCESSORS(UCPWeaponAttributeSet, MeeleAttackRangeMultiplier);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_MeeleAttackRangeMultiplier(const FGameplayAttributeData& OldMeeleAttackRangeMultiplier);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StunValue;

	UPROPERTY(ReplicatedUsing = OnRep_MeeleAttackRangeMultiplier, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MeeleAttackRangeMultiplier;
};
