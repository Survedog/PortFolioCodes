// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "CPCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UCPCharacterAnimInstance : public UAnimInstance, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UCPCharacterAnimInstance();

	virtual void SetCharacterASC(class UAbilitySystemComponent* InASC);
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void DeadTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TWeakObjectPtr<class ACPCharacterBase> CPCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TWeakObjectPtr<class UCharacterMovementComponent> MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> CharacterASC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovementYawRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsDead : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float JumpThreshold;
};
