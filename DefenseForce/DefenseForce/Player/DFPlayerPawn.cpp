// Copyright 2024, Survedog. All rights reserved.


#include "Player/DFPlayerPawn.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Player/DFPlayerController.h"
#include "Structure/DFTowerBase.h"
#include "AbilitySystemComponent.h"
#include "DefenseForce.h"
#include "DFLog.h"

ADFPlayerPawn::ADFPlayerPawn() : PlayerAimLocation(FVector::Zero()), CurrentStructureUnderCursor(nullptr), CurrentControlledTower(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	PlayerAimTraceLength = 100000.0f;
}

AActor* ADFPlayerPawn::GetAttackerActor() const
{
	return CurrentControlledTower;
}

UAbilitySystemComponent* ADFPlayerPawn::GetAttackerActorASC() const
{
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

void ADFPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ADFPlayerPawn, PlayerAimLocation, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ADFPlayerPawn, CurrentControlledTower, COND_None);
}

void ADFPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	DFPlayerController = CastChecked<ADFPlayerController>(NewController);
	IAbilitySystemInterface* PSGASInterface = Cast<IAbilitySystemInterface>(NewController->PlayerState);
	if (PSGASInterface)
	{
		ASC = PSGASInterface->GetAbilitySystemComponent();
		ensure(ASC.Get());
		ASC->InitAbilityActorInfo(NewController->PlayerState, this);

		// Give Abilities
		for (auto NonInputAbility : NonInputAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(NonInputAbility);
			ASC->GiveAbility(AbilitySpec);
		}

		for (auto InputAbilityPair : InputAbilityMap)
		{
			FGameplayAbilitySpec AbilitySpec(InputAbilityPair.Value);
			AbilitySpec.InputID = static_cast<int32>(InputAbilityPair.Key);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void ADFPlayerPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	IAbilitySystemInterface* PSGASInterface = Cast<IAbilitySystemInterface>(GetPlayerState());
	if (PSGASInterface)
	{
		ASC = PSGASInterface->GetAbilitySystemComponent();
		ensure(ASC.Get());
		ASC->InitAbilityActorInfo(GetPlayerState(), this);
	}
}

void ADFPlayerPawn::OnRep_Controller()
{
	Super::OnRep_Controller();

	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	if (Controller && Controller != DFPlayerController)
	{
		DFPlayerController = CastChecked<ADFPlayerController>(Controller);
	}
}

void ADFPlayerPawn::AbilityInputPressed(EDFAbilityInputID InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	if (Spec)
	{
		Spec->InputPressed = true;

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle, true);
		}
	}
}

void ADFPlayerPawn::AbilityInputReleased(EDFAbilityInputID InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

bool ADFPlayerPawn::TryActivateAbilityOfClass(TSubclassOf<UGameplayAbility> InAbilityClass, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(InAbilityClass);
	if (Spec && !Spec->IsActive())
	{
		return ASC->TryActivateAbility(Spec->Handle, bAllowRemoteActivation);
	}
	return false;
}

void ADFPlayerPawn::CancelAbilityOfClass(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(InAbilityClass);
	if (Spec && Spec->IsActive())
	{
		ASC->CancelAbilityHandle(Spec->Handle);
	}
}

int32 ADFPlayerPawn::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	return ASC->HandleGameplayEvent(EventTag, &Payload);
}

void ADFPlayerPawn::StartTowerControl(ADFTowerBase* NewTower)
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	if (!NewTower->IsBeingControlled())
	{
		if (NewTower != CurrentControlledTower)
		{
			if (CurrentControlledTower)
			{
				EndTowerControl();
			}
			CurrentControlledTower = NewTower;
			OnTowerControlStartCallback(NewTower);
		}
	}
}

void ADFPlayerPawn::EndTowerControl()
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	if (CurrentControlledTower && CurrentControlledTower->IsBeingControlled())
	{
		OnTowerControlEndCallback(CurrentControlledTower);
		CurrentControlledTower = nullptr;
	}
}

void ADFPlayerPawn::OnTowerControlStartCallback_Implementation(ADFTowerBase* NewControlledTower)
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	CurrentControlledTower->SetOwner(this);
	CurrentControlledTower->OnControlStart(this);
	OnTowerControlStart.Broadcast(CurrentControlledTower);
}

void ADFPlayerPawn::OnTowerControlEndCallback_Implementation(ADFTowerBase* LastControlledTower)
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	LastControlledTower->OnControlEnd(this);
	LastControlledTower->SetOwner(nullptr);
	OnTowerControlEnd.Broadcast(LastControlledTower);
}

void ADFPlayerPawn::OnBeginCursorOverStructureCallback_Implementation(AActor* TouchedActor)
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	CurrentStructureUnderCursor = Cast<ADFStructureBase>(TouchedActor);
}

void ADFPlayerPawn::OnEndCursorOverStructureCallback_Implementation(AActor* TouchedActor)
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	if (CurrentStructureUnderCursor == TouchedActor)
	{
		CurrentStructureUnderCursor = nullptr;
	}
}

void ADFPlayerPawn::OnRep_CurrentControlledTower(ADFTowerBase* LastControlledTower)
{
	DF_NETLOG(LogDFNET, Log, TEXT("Start"));
	if (CurrentControlledTower)
	{
		OnTowerControlStartCallback(CurrentControlledTower);
	}
	else
	{
		OnTowerControlEndCallback(LastControlledTower);
	}
}

UAbilitySystemComponent* ADFPlayerPawn::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
