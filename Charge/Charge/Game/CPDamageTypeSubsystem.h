// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CPDamageTypeSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Default,
	Explosion,
	Fire,
};

/**
 * 
 */
UCLASS()
class CHARGE_API UCPDamageTypeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UCPDamageTypeSubsystem* Get(const UWorld* World);

	const TSubclassOf<UDamageType> GetDamageTypeFromEnum(EDamageType EnumValue);
	EDamageType GetEnumFromDamageType(TSubclassOf<UDamageType> DamageType);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	//virtual void Deinitialize();

	UPROPERTY()
	TMap<TSubclassOf<UDamageType>, EDamageType> DamageTypeEnumMap;
};
