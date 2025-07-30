// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CPCharacterBaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHpZeroDelegate);

/**
 * 
 */
UCLASS()
class CHARGE_API UCPCharacterBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCPCharacterBaseAttributeSet();

	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, Hp);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, MaxHp);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, DamageToApply);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, StunGauge);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, MaxStunGauge);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, HitInvincibleTime);
	ATTRIBUTE_ACCESSORS(UCPCharacterBaseAttributeSet, MoveSpeed);

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Meta = (HideInDetailsView))
	mutable FOnHpZeroDelegate OnHpZero;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Hp, BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Hp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHp, BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHp;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageToApply;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StunGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStunGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HitInvincibleTime;

	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;

protected:
	uint8 bIsDead : 1;	

protected:
	UFUNCTION()
	void OnRep_Hp(const FGameplayAttributeData& OldHp);

	UFUNCTION()
	void OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

protected:
	void ApplyInvincibleEffect(float InvincibleTime);

	UPROPERTY(Meta = (HideInDetailsView))
	TSubclassOf<UGameplayEffect> InvincibleEffect;
};
