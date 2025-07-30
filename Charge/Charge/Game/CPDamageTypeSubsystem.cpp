// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CPDamageTypeSubsystem.h"
#include "Weapon/DamageTypes/DamageType_Explosion.h"
#include "Weapon/DamageTypes/DamageType_Fire.h"
#include "Charge.h"

UCPDamageTypeSubsystem* UCPDamageTypeSubsystem::Get(const UWorld* World)
{
	if (World)
	{
		return UGameInstance::GetSubsystem<UCPDamageTypeSubsystem>(World->GetGameInstance());
	}
	return nullptr;
}

const TSubclassOf<UDamageType> UCPDamageTypeSubsystem::GetDamageTypeFromEnum(EDamageType EnumValue)
{
	const TSubclassOf<UDamageType>* FindResult = DamageTypeEnumMap.FindKey(EnumValue);
	if (FindResult)
	{
		return *FindResult;
	}
	else
	{
		CP_LOG(LogCP, Error, TEXT("Input EDamageType value is not valid."));
		return nullptr;
	}
}

EDamageType UCPDamageTypeSubsystem::GetEnumFromDamageType(TSubclassOf<UDamageType> DamageType)
{
	EDamageType* FindResult = DamageTypeEnumMap.Find(DamageType);
	if (FindResult)
	{
		return *FindResult;
	}
	else
	{
		CP_LOG(LogCP, Error, TEXT("Input DamageTypeClass is not valid."));
		return EDamageType::Default;
	}
}

void UCPDamageTypeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	DamageTypeEnumMap.Add(UDamageType::StaticClass(), EDamageType::Default);
	DamageTypeEnumMap.Add(UDamageType_Explosion::StaticClass(), EDamageType::Explosion);
	DamageTypeEnumMap.Add(UDamageType_Fire::StaticClass(), EDamageType::Fire);
}
