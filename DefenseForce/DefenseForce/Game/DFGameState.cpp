// Copyright 2024, Survedog. All rights reserved.


#include "Game/DFGameState.h"
#include "Net/UnrealNetwork.h"
#include "DFLog.h"


ADFGameState::ADFGameState()
{
	CurrentMoneyAmount = 0.0f;
	bIsGameOver = false;
}

void ADFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ADFGameState, CurrentMoneyAmount, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ADFGameState, bIsGameOver, COND_None, REPNOTIFY_OnChanged);
}

void ADFGameState::OnRep_CurrentMoneyAmount()
{
	OnCurrentMoneyAmountChange.Broadcast(CurrentMoneyAmount);
}

void ADFGameState::OnRep_IsGameOver()
{
	OnGameOverCallback();
}

void ADFGameState::OnGameOverCallback_Implementation()
{
	DF_NETLOG(LogDF, Log, TEXT("GameOver"));
}
