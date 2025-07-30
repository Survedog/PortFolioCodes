// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DFCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UDFCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDFCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UDFCharacterAttributeSet, MoveSpeed);

protected:
	//virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	//virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;

protected:
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
