// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CPStaminaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfStaminaDelegate);

/**
 * 
 */
UCLASS()
class CHARGE_API UCPStaminaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCPStaminaAttributeSet();

	ATTRIBUTE_ACCESSORS(UCPStaminaAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UCPStaminaAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UCPStaminaAttributeSet, StaminaRecoverySpeed);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Meta = (HideInDetailsView))
	mutable FOutOfStaminaDelegate OnOutOfStamina;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Stamina, BlueprintReadOnly, Category = "Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(ReplicatedUsing = OnRep_MaxStamina, BlueprintReadOnly, Category = "Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(ReplicatedUsing = OnRep_StaminaRecoverySpeed, BlueprintReadOnly, Category = "Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaRecoverySpeed;

protected:
	void OnStaminaConsumingEndCallback(const FGameplayTag Tag, int32 NewTagCount);
	void StartStaminaRecovery();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Stamina", Meta = (HideInDetailsView))
	TSubclassOf<class UGameplayEffect> StaminaRecoveryEffect;

	uint8 bOutOfStamina : 1;
	FTimerHandle StaminaRecoveryTimerHandle;
	FActiveGameplayEffectHandle ActiveStaminaRecoveryEffectHandle;
	float StaminaRecoveryCooldown;	

protected:
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	void OnRep_StaminaRecoverySpeed(const FGameplayAttributeData& OldStaminaRecoverySpeed);
};
