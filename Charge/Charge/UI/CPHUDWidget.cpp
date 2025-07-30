// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CPHUDWidget.h"
#include "Components/Image.h"
#include "UI/GASUserWidget.h"
#include "UI/WeaponInventoryUserWidget.h"
#include "Character/CPPlayerCharacter.h"
#include "Charge.h"

void UCPHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CPPlayerCharacter = CastChecked<ACPPlayerCharacter>(GetOwningPlayerPawn());
	CrossHairImage = CastChecked<UImage>(GetWidgetFromName(TEXT("ImgCrosshair")));
	WeaponInventorySlots = CastChecked<UWeaponInventoryUserWidget>(GetWidgetFromName(TEXT("WeaponInventorySlots")));
}

void UCPHUDWidget::NativeConstruct()
{
	CPPlayerCharacter->SetupHUDWidget(this);
	WeaponInventorySlots->SetWeaponComponent(CPPlayerCharacter->GetCharacterWeaponComponent());

	if (CrossHairImage)
	{		
		SetCrosshairVisibility(false);
	}
}

void UCPHUDWidget::SetGASWidgetASC(UAbilitySystemComponent* InASC)
{
	CP_NETUILOG(LogCPUI, Log, TEXT("Start"));
	for (FName& GASWidgetName : GASUserWidgetNames)
	{
		UGASUserWidget* GASWidget = Cast<UGASUserWidget>(GetWidgetFromName(GASWidgetName));
		if (GASWidget)
		{
			GASWidget->SetAbilitySystemComponent(InASC);
		}
	}
}

void UCPHUDWidget::SetCrosshairVisibility(bool bInIsVisible)
{
	if (CrossHairImage)
	{
		ESlateVisibility CrossHairVisibility = bInIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		CrossHairImage->SetVisibility(CrossHairVisibility);
	}
}