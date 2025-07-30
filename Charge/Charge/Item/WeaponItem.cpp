// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponItem.h"
#include "Object/InteractTriggerComponent.h"
#include "Character/CPPlayerCharacter.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Charge.h"

// Sets default values
AWeaponItem::AWeaponItem()
{
	bReplicates = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ItemMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	RootComponent = ItemMesh;

	InteractTrigger = CreateDefaultSubobject<UInteractTriggerComponent>(TEXT("InteractTrigger"));
	InteractTrigger->SetupAttachment(ItemMesh);
}

void AWeaponItem::BeginPlay()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::BeginPlay();

	SetWeaponClass(WeaponClass);
}

void AWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponItem, WeaponClass);
}

void AWeaponItem::SetWeaponClass(TSubclassOf<UWeapon> NewWeaponClass)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	WeaponClass = NewWeaponClass;
	if (WeaponClass)
	{
		UWeapon* WeaponCDO = CastChecked<UWeapon>(NewWeaponClass->GetDefaultObject());
		ChangeWeaponMesh(WeaponCDO->GetWeaponMesh());
	}
}

void AWeaponItem::OnInteract_Implementation(ACPPlayerCharacter* InteractedCharacter)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	UCharacterWeaponComponent* WeaponComponent = InteractedCharacter->GetCharacterWeaponComponent();
	bool bIsObtained = WeaponComponent->ObtainWeapon(WeaponClass);
	if (bIsObtained)
	{
		Destroy();
	}
}

void AWeaponItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == TEXT("WeaponClass"))
	{
		SetWeaponClass(WeaponClass);
	}
}

void AWeaponItem::ChangeWeaponMesh(TSoftObjectPtr<UStaticMesh> InWeaponMesh)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	if (InWeaponMesh.IsNull())
	{
		ItemMesh->SetStaticMesh(nullptr);
	}
	else
	{
		if (InWeaponMesh.IsPending())
		{
			InWeaponMesh.LoadSynchronous();
		}
		ItemMesh->SetStaticMesh(InWeaponMesh.Get());
	}
}

void AWeaponItem::OnRep_WeaponClass()
{
	SetWeaponClass(WeaponClass);
}
