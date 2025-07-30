// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/CharacterWeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Character/CPCharacterBase.h"
#include "GameFramework/PlayerState.h"
#include "Projectile/CPProjectileBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASTag/CPGameplayTag.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Item/WeaponItem.h"
#include "Charge.h"

UCharacterWeaponComponent::UCharacterWeaponComponent() : CurrentWeaponIndex(-1), WeaponToSwapIndex(0)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	//bReplicateUsingRegisteredSubObjectList = true;

	// Weapon
	WeaponInventory.Reserve(MaxWeaponAmount);
	for (int i = 0; i < MaxWeaponAmount; ++i)
	{
		WeaponInventory.Add(nullptr);
	}

}

// Need to set bWantsInitializeComponent true.
void UCharacterWeaponComponent::InitializeComponent()
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	Super::InitializeComponent();

	CPCharacterOwner = CastChecked<ACPCharacterBase>(GetOwner());
}

void UCharacterWeaponComponent::BeginPlay()
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	Super::BeginPlay();
}

void UCharacterWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterWeaponComponent, CurrentWeaponIndex);
	DOREPLIFETIME(UCharacterWeaponComponent, WeaponInventory);
}

bool UCharacterWeaponComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (UWeapon* Weapon : WeaponInventory)
	{
		bWroteSomething |= Channel->ReplicateSubobject(Weapon, *Bunch, *RepFlags);
	}
	return bWroteSomething;
}

void UCharacterWeaponComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == TEXT("WeaponToSwapIndex"))
	{
		if (HasBegunPlay() && CPCharacterOwner.IsValid() && CPCharacterOwner->HasAuthority())
		{
			SwapCurrentWeapon(WeaponToSwapIndex, CPCharacterOwner->GetAbilitySystemComponent());
		}
	}
}

void UCharacterWeaponComponent::OnOwnerASCSet(UAbilitySystemComponent* TargetASC)
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	for (const TSubclassOf<UWeapon>& WeaponClass : InitialWeaponClassess)
	{
		ObtainWeapon(WeaponClass, false);
	}
	SwapCurrentWeapon(WeaponToSwapIndex, TargetASC);
}

bool UCharacterWeaponComponent::ObtainWeapon(const TSubclassOf<UWeapon>& NewWeaponClass, bool bSwapToNewWeapon)
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	if (!NewWeaponClass)
	{
		return false;
	}

	UWeapon* NewWeapon = NewObject<UWeapon>(this, NewWeaponClass);
	for (int32 WeaponIndex = 0; WeaponIndex < WeaponInventory.Num(); WeaponIndex++)
	{
		if (!WeaponInventory[WeaponIndex])
		{
			NewWeapon->OnWeaponObtained(this);
			WeaponInventory[WeaponIndex] = NewWeapon;

			if (bSwapToNewWeapon)
			{
				UAbilitySystemComponent* ASC = CPCharacterOwner.IsValid() ? CPCharacterOwner->GetAbilitySystemComponent() : nullptr;
				SwapCurrentWeapon(WeaponIndex, ASC, true);
			}
			OnWeaponInventoryChange.ExecuteIfBound(WeaponInventory);
			return true;
		}
	}
	return false;
}

void UCharacterWeaponComponent::ObtainWeaponFromClass_EditorFunc()
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	if (NewWeaponClass_ForEditorFunc)
	{
		if (CPCharacterOwner.IsValid())
		{
			ObtainWeapon(NewWeaponClass_ForEditorFunc, false);
		}
		else
		{
			CP_NETSUBLOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		}
	}
}

bool UCharacterWeaponComponent::DropWeapon(int32 DropWeaponIndex)
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	if (WeaponInventory[DropWeaponIndex])
	{	
		UWeapon* DroppedWeapon = WeaponInventory[DropWeaponIndex];
		WeaponInventory[DropWeaponIndex] = nullptr;
		SwapCurrentWeapon(CurrentWeaponIndex, CPCharacterOwner->GetAbilitySystemComponent(), true);
		DroppedWeapon->OnWeaponDropped(this);
		OnWeaponInventoryChange.ExecuteIfBound(WeaponInventory);

		if (CPCharacterOwner->HasAuthority())
		{
			const FVector& ItemSpawnLocation = GetOwner()->GetActorLocation();
			AWeaponItem* DroppedWeaponItem = GetWorld()->SpawnActorDeferred<AWeaponItem>(AWeaponItem::StaticClass(), FTransform::Identity, nullptr, CPCharacterOwner.Get());
			DroppedWeaponItem->SetWeaponClass(DroppedWeapon->GetClass());
			DroppedWeaponItem->FinishSpawning(FTransform(ItemSpawnLocation));
		}
		return true;
	}
	return false;
}

void UCharacterWeaponComponent::SwapCurrentWeapon(int32 NewCurrentWeaponIndex, UAbilitySystemComponent* TargetASC, bool bIsNewWeapon)
{
	CP_NETSUBLOG(LogCP, Log, TEXT("Start"));
	if (CurrentWeaponIndex == NewCurrentWeaponIndex && !bIsNewWeapon)
	{
		return;
	}

	if (WeaponInventory.IsValidIndex(NewCurrentWeaponIndex))
	{
		
		UWeapon* OldEquippedWeapon = WeaponInventory.IsValidIndex(CurrentWeaponIndex) ? WeaponInventory[CurrentWeaponIndex] : nullptr;
		UWeapon* NewEquippedWeapon = WeaponInventory[NewCurrentWeaponIndex];
		CurrentWeaponIndex = NewCurrentWeaponIndex;

		if (OldEquippedWeapon && OldEquippedWeapon != NewEquippedWeapon)
		{
			OldEquippedWeapon->OnWeaponUnequipped();
		}

		if (!NewEquippedWeapon)
		{
			NewEquippedWeapon = CastChecked<UWeapon>(UWeapon::StaticClass()->GetDefaultObject());
		}
		NewEquippedWeapon->OnWeaponEquipped();
		ApplyWeaponMesh(NewEquippedWeapon);

		if (GetOwnerRole() == ROLE_Authority)
		{
			// Init Attributes
			if (TargetASC && HasBegunPlay())
			{
				FGameplayEffectSpecHandle InitStatEffectSpecHandle = TargetASC->MakeOutgoingSpec(InitWeaponStatEffect, 1.0f, FGameplayEffectContextHandle());
				if (InitStatEffectSpecHandle.IsValid())
				{
					InitStatEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_MaxChargeAmount, NewEquippedWeapon->GetBaseMaxChargeAmount());
					InitStatEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_ChargeSpeed, NewEquippedWeapon->GetBaseChargeSpeed());
					InitStatEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_Damage, NewEquippedWeapon->GetBaseDamage());
					InitStatEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_StunValue, NewEquippedWeapon->GetBaseStunValue());
					TargetASC->BP_ApplyGameplayEffectSpecToSelf(InitStatEffectSpecHandle);
				}
			}
		}
		OnSwapCurrentWeapon.Broadcast(NewCurrentWeaponIndex);
	}
	else
	{
		CP_LOG(LogCP, Error, TEXT("NewCurrentWeaponIndex value is not a valid index for WeaponInventory."));
	}
}

void UCharacterWeaponComponent::OnRep_CurrentWeaponIndex()
{
	CP_NETSUBLOG(LogCPNET, Log, TEXT("Start"));
	SwapCurrentWeapon(CurrentWeaponIndex, CPCharacterOwner->GetAbilitySystemComponent(), true);
}

void UCharacterWeaponComponent::OnRep_WeaponInventory()
{
	CP_NETSUBLOG(LogCPNET, Log, TEXT("Start"));
	for (UWeapon* Weapon : WeaponInventory)
	{
		if (Weapon && !Weapon->GetCPCharacterOwner())
		{
			Weapon->OnWeaponObtained(this);
		}
	}
	SwapCurrentWeapon(CurrentWeaponIndex, CPCharacterOwner->GetAbilitySystemComponent(), true);
	OnWeaponInventoryChange.ExecuteIfBound(WeaponInventory);
}

void UCharacterWeaponComponent::ApplyWeaponMesh(const UWeapon* InWeapon)
{
	if (InWeapon)
	{
		if (InWeapon->GetWeaponMesh().IsPending())
		{
			InWeapon->GetWeaponMesh().LoadSynchronous();
		}
		UStaticMesh* NewMeshData = InWeapon->GetWeaponMesh().Get();
		const FTransform& NewMeshRelativeTransform = InWeapon->GetMeshRelativeTransform();
		CPCharacterOwner->SetWeaponMesh(NewMeshData, NewMeshRelativeTransform);
	}
	else
	{
		CPCharacterOwner->SetWeaponMesh(nullptr);
	}
}
