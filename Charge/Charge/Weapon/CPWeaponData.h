// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponType.h"
#include "CPWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UCPWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UCPWeaponData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	TSubclassOf<class ACPProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	TObjectPtr<UAnimMontage> MeeleAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon, meta = (ClampMin = 0, ClampMax = 127))
	int32 MaxMeeleAttackComboCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	FVector RelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	FRotator RelativeRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	float ChargeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	float MaxChargeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterWeapon)
	float Damage;
};
