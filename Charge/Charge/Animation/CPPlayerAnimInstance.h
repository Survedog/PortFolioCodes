// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CPCharacterAnimInstance.h"
#include "Weapon/WeaponType.h"
#include "CPPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UCPPlayerAnimInstance : public UCPCharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	UCPPlayerAnimInstance();

	virtual void SetCharacterASC(class UAbilitySystemComponent* InASC) override;	

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void AimTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount);
	void ChargeTagChangeEventCallback(const FGameplayTag Tag, int32 NewTagCount);

	void SwapCurrentWeaponCallback(uint32 NewCurrentWeaponIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TWeakObjectPtr<class ACPPlayerCharacter> CPPlayerCharacter;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float AimRotationPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsAiming : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsCharging : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	EWeaponType EquippedWeaponType;
};
