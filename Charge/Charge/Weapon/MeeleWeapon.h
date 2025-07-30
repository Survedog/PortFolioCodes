// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "GameplayTagContainer.h"
#include "MeeleWeapon.generated.h"

USTRUCT(BlueprintType)
struct FMeeleAttackHitTriggerInfo
{
	GENERATED_BODY()

public:
	FMeeleAttackHitTriggerInfo() : CapsuleHalfHeight(1.0f), CapsuleRadius(1.0f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capsule)
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capsule)
	float CapsuleRadius;
};

/**
 * 
 */
UCLASS()
class CHARGE_API UMeeleWeapon : public UWeapon
{
	GENERATED_BODY()
	
public:
	UMeeleWeapon();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void OnWeaponObtained_Implementation(class UCharacterWeaponComponent* NewWeaponComp) override;
	virtual void OnWeaponDropped_Implementation(class UCharacterWeaponComponent* OldWeaponComp) override;

	virtual void MeeleAttackHitCheck(const uint8 TriggerIndex);
	void ResetLastAttackTriggerLocation();

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UDamageType> GetMeeleAttackDamageType() const;

	UFUNCTION(BlueprintNativeEvent)
	FGameplayTagContainer GetDamageReactionAbilityTag() const;
	FGameplayTagContainer GetDamageReactionAbilityTag_Implementation() const { return FGameplayTagContainer(); }

protected:
	virtual void PrimaryAttack_Implementation(int32 InComboCount = 1) override;
	virtual void EndPrimaryAttack_Implementation() override;	
	
	FORCEINLINE void ResetMeeleAttackHitTargets() { MeeleAttackHitTargets.Reset(); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TArray<FMeeleAttackHitTriggerInfo> MeeleAttackHitTriggers;

	UPROPERTY(VisibleInstanceOnly, Category = Combat)
	TSet<TWeakObjectPtr<const AActor>> MeeleAttackHitTargets;	

	TArray<FVector> LastAttackTriggerLocation;
};
