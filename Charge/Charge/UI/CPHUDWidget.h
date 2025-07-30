// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UCPHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGASWidgetASC(class UAbilitySystemComponent* InASC);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

// GAS Widget
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FName> GASUserWidgetNames;

// Crosshair
public:
	inline void SetCrosshairVisibility(bool bInIsVisible);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> CrossHairImage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWeaponInventoryUserWidget> WeaponInventorySlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class ACPPlayerCharacter> CPPlayerCharacter;
};
