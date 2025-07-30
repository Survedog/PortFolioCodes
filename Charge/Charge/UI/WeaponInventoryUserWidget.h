// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInventoryUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UWeaponInventoryUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetWeaponComponent(class UCharacterWeaponComponent* NewWeaponComponent);

protected:
	void UpdateWeaponSlots(const TArray<TObjectPtr<class UWeapon>>& InWeaponInventory);
	void UpdateCurrentWeapon(uint32 NewCurrentWeaponIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<class UCharacterWeaponComponent> WeaponComponent;

	UPROPERTY()
	TArray<TObjectPtr<class UImage>> WeaponSlotImages;

	UPROPERTY()
	TArray<TObjectPtr<class UBorder>> WeaponSlotBorders;
};
