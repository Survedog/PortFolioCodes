// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPPlayerController.h"
#include "Player/CPPlayerState.h"
#include "Character/CPPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "UI/CPHUDWidget.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Animation/CPPlayerAnimInstance.h"
#include "Charge.h"

ACPPlayerController::ACPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UCPHUDWidget> HUDWidgetClassRef(TEXT("/Game/Charge/Blueprint/UI/WBP_HUD.WBP_HUD_C"));
	if (HUDWidgetClassRef.Class)
	{
		HUDWidgetClass = HUDWidgetClassRef.Class;
	}
}

void ACPPlayerController::BeginPlay()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::BeginPlay();
}

void ACPPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	ACPPlayerState* CPPlayerState = GetPlayerState<ACPPlayerState>();
	ensure(CPPlayerState);
	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCPHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->SetGASWidgetASC(CPPlayerState->GetAbilitySystemComponent());	
			HUDWidget->AddToViewport();
		}
	}
}

void ACPPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	ACPPlayerState* CPPlayerState = GetPlayerState<ACPPlayerState>();
	if (CPPlayerState && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCPHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->SetGASWidgetASC(CPPlayerState->GetAbilitySystemComponent());
			HUDWidget->AddToViewport();
		}
	}
}
