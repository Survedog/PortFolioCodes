// Copyright 2024, Survedog. All rights reserved.


#include "Player/DFPlayerController.h"
#include "Player/DFPlayerPawn.h"
#include "AbilitySystemComponent.h"
#include "Structure/DFTowerBase.h"
#include "Net/UnrealNetwork.h"
#include "Game/DFGameState.h"
#include "GAS/DFGameplayTags.h"
#include "DFLog.h"

ADFPlayerController::ADFPlayerController() : DFPlayerPawn(nullptr)
{
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

AActor* ADFPlayerController::GetAttackerActor() const
{
	return GetCurrentControlledTower();
}

UAbilitySystemComponent* ADFPlayerController::GetAttackerActorASC() const
{
	ADFTowerBase* CurrentControlledTower = GetCurrentControlledTower();
	if (CurrentControlledTower)
	{
		IAbilitySystemInterface* TowerGASInterface = Cast<IAbilitySystemInterface>(CurrentControlledTower);
		if (TowerGASInterface)
		{
			return TowerGASInterface->GetAbilitySystemComponent();
		}
	}
	return nullptr;
}

void ADFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	DF_NETLOG(LogDF, Log, TEXT("Start"));
}

void ADFPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	DFPlayerPawn = CastChecked<ADFPlayerPawn>(aPawn);
	DFPlayerPawn->OnTowerControlStart.AddUniqueDynamic(this, &ADFPlayerController::OnTowerControlStartCallback);
	DFPlayerPawn->OnTowerControlEnd.AddUniqueDynamic(this, &ADFPlayerController::OnTowerControlEndCallback);
}

void ADFPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	DFPlayerPawn = CastChecked<ADFPlayerPawn>(P);
}

void ADFPlayerController::StartTowerControl(ADFTowerBase* NewTower)
{
	IPlayerTowerControlInterface* TowerControlInterface = Cast<IPlayerTowerControlInterface>(GetPawn());
	if (TowerControlInterface)
	{
		TowerControlInterface->StartTowerControl(NewTower);
	}
}

void ADFPlayerController::EndTowerControl()
{
	IPlayerTowerControlInterface* TowerControlInterface = Cast<IPlayerTowerControlInterface>(GetPawn());
	if (TowerControlInterface)
	{
		TowerControlInterface->EndTowerControl();
	}
}

ADFTowerBase* ADFPlayerController::GetCurrentControlledTower() const
{
	IPlayerTowerControlInterface* TowerControlInterface = Cast<IPlayerTowerControlInterface>(GetPawn());
	if (TowerControlInterface)
	{
		return TowerControlInterface->GetCurrentControlledTower();
	}
	return nullptr;
}

ADFStructureBase* ADFPlayerController::GetCurrentStructureUnderCursor() const
{
	IPlayerTowerControlInterface* TowerControlInterface = Cast<IPlayerTowerControlInterface>(GetPawn());
	if (TowerControlInterface)
	{
		return TowerControlInterface->GetCurrentStructureUnderCursor();
	}
	return nullptr;
}

FVector ADFPlayerController::GetPlayerAimLocation() const
{
	return DFPlayerPawn->GetPlayerAimLocation();
}

void ADFPlayerController::SetPlayerAimLocation(const FVector& InPlayerAimLocation)
{
	DFPlayerPawn->SetPlayerAimLocation(InPlayerAimLocation);
}

void ADFPlayerController::EnterBuildMode(TSubclassOf<ADFStructureBase> InTargetStructureClass)
{
	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	if (!CanBuildStructure(InTargetStructureClass))
	{
		DF_NETLOG(LogDFNET, Log, TEXT("Can't afford the build cost of target structure."));
		return;
	}

	if (DFPlayerPawn)
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;
		Payload.Target = DFPlayerPawn;
		Payload.OptionalObject = InTargetStructureClass->GetDefaultObject();
		DFPlayerPawn->GetAbilitySystemComponent()->HandleGameplayEvent(GASTAG_Player_Action_BuildStructure, &Payload);
	}
	
	OnEnterBuildMode(InTargetStructureClass);
}

void ADFPlayerController::ExitBuildMode(ADFStructureBase* InBuiltStructure)
{
	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	OnExitBuildMode(InBuiltStructure);
}

bool ADFPlayerController::CanBuildStructure(TSubclassOf<ADFStructureBase> InTargetStructureClass) const
{
	const float BuildCost = CastChecked<ADFStructureBase>(InTargetStructureClass->GetDefaultObject())->GetBuildCost();
	const float CurrentMoneyAmount = CastChecked<ADFGameState>(GetWorld()->GetGameState())->GetCurrentMoneyAmount();
	return CurrentMoneyAmount >= BuildCost;
}
