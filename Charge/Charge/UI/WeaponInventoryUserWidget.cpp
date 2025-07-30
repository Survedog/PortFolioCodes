// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponInventoryUserWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

void UWeaponInventoryUserWidget::SetWeaponComponent(UCharacterWeaponComponent* NewWeaponComponent)
{
	CP_NETUILOG(LogCPUI, Log, TEXT("Start"));
	WeaponComponent = NewWeaponComponent;
	if (WeaponComponent.IsValid())
	{
		const uint8 MaxWeaponAmount = WeaponComponent->GetMaxWeaponAmount();
		for (uint8 Idx = 0; Idx < MaxWeaponAmount; Idx++)
		{
			FString ImageWidgetName = FString::Printf(TEXT("ImgWeaponSlot%d"), Idx);
			WeaponSlotImages.Add(CastChecked<UImage>(GetWidgetFromName(*ImageWidgetName)));

			FString BorderWidgetName = FString::Printf(TEXT("BorderWeaponSlot%d"), Idx);
			WeaponSlotBorders.Add(CastChecked<UBorder>(GetWidgetFromName(*BorderWidgetName)));
		}

		WeaponComponent->OnWeaponInventoryChange.BindUObject(this, &UWeaponInventoryUserWidget::UpdateWeaponSlots);
		WeaponComponent->OnSwapCurrentWeapon.AddUObject(this, &UWeaponInventoryUserWidget::UpdateCurrentWeapon);

		UpdateWeaponSlots(WeaponComponent->GetWeaponInventory());
		UpdateCurrentWeapon(WeaponComponent->GetCurrentWeaponIndex());
	}
}

void UWeaponInventoryUserWidget::UpdateWeaponSlots(const TArray<TObjectPtr<class UWeapon>>& InWeaponInventory)
{
	CP_NETUILOG(LogCPUI, Log, TEXT("Start"));
	for (uint8 Idx = 0; Idx < InWeaponInventory.Num(); Idx++)
	{
		if (InWeaponInventory[Idx])
		{
			if (TSoftObjectPtr<UTexture2D> TexturePtr = InWeaponInventory[Idx]->GetWeaponThumbnailTexture())
				WeaponSlotImages[Idx]->SetBrushFromSoftTexture(TexturePtr);

			WeaponSlotImages[Idx]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			WeaponSlotImages[Idx]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWeaponInventoryUserWidget::UpdateCurrentWeapon(uint32 NewCurrentWeaponIndex)
{
	CP_NETUILOG(LogCPUI, Log, TEXT("Start"));
	for (uint8 Idx = 0; Idx < WeaponSlotBorders.Num(); Idx++)
	{
		if (Idx == NewCurrentWeaponIndex)
		{
			WeaponSlotBorders[Idx]->SetBrushColor(FLinearColor::Yellow);
		}
		else
		{
			WeaponSlotBorders[Idx]->SetBrushColor(FLinearColor::White);
		}
	}
}
