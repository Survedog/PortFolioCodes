// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/CPWeaponData.h"

UCPWeaponData::UCPWeaponData() : WeaponType(EWeaponType::None), WeaponMesh(nullptr), ProjectileClass(nullptr), MeeleAttackMontage(nullptr), RelativeLocation(FVector::ZeroVector), RelativeRotation(FRotator::ZeroRotator)
{
	MaxMeeleAttackComboCount = 1;
	ChargeSpeed = 0.0f;
	MaxChargeAmount = 100.0f;
	Damage = 0.0f;
}
