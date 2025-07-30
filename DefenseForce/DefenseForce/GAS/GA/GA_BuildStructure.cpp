// Copyright 2024, Survedog. All rights reserved.


#include "GAS/GA/GA_BuildStructure.h"
#include "GAS/AT/DFAT_WaitTargetData_ReusableTA.h"
#include "GAS/TA/DFGATA_ActorPlacement.h"
#include "GAS/TA/Reticle/GAWorldReticle_ActorVisualization.h"
#include "Subsystem/TargetingInstanceSubsystem.h"
#include "Structure/DFStructureBase.h"
#include "Interface/PlayerBuildModeInterface.h"
#include "Net/UnrealNetwork.h"
#include "Game/DFGameState.h"
#include "AbilitySystemComponent.h"
#include "Physics/DFCollision.h"
#include "DFLog.h"

UGA_BuildStructure::UGA_BuildStructure() : TargetStructureClass(nullptr), BuiltStructure(nullptr), DFActorPlacementTA(nullptr)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	static ConstructorHelpers::FClassFinder<ADFGATA_ActorPlacement> TargetActorClassRef(TEXT("/Game/DefenseForce/Blueprint/GAS/TA/BP_DFGATA_ActorPlacement.BP_DFGATA_ActorPlacement_C"));
	if (TargetActorClassRef.Class)
	{
		ActorPlacementTAClass = TargetActorClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<AGAWorldReticle_ActorVisualization> ReticleClassRef(TEXT("/Game/DefenseForce/Blueprint/GAS/TA/ReticleActor/BP_GAWorldReticle_ActorVisualization.BP_GAWorldReticle_ActorVisualization_C"));
	if (ReticleClassRef.Class)
	{
		ActorVisualReticleClass = ReticleClassRef.Class;
	}	
}

void UGA_BuildStructure::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGA_BuildStructure, BuiltStructure, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UGA_BuildStructure::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	
	BuiltStructure = nullptr;

	if (IsLocallyControlled())
	{
		if (!DFActorPlacementTA)
		{		
			UTargetingInstanceSubsystem* TargetingInstanceSubsystem = ULocalPlayer::GetSubsystem<UTargetingInstanceSubsystem>(ActorInfo->PlayerController->GetLocalPlayer());
			DFActorPlacementTA = TargetingInstanceSubsystem->GetTargetActorFromClass<ADFGATA_ActorPlacement>(ActorPlacementTAClass);
			DFActorPlacementTA->SetTraceProfile(CPROFILE_BlockOnlyWorld);
			DFActorPlacementTA->SetIgnoreBlockingHits(false);
			DFActorPlacementTA->SetStartTraceFromPlayerCamera(true);
			DFActorPlacementTA->SetTraceTowardMouseAimLocation(true);
			DFActorPlacementTA->ReticleClass = ActorVisualReticleClass;
		}

		TargetStructureClass = TriggerEventData->OptionalObject->GetClass();
		if (TargetStructureClass != DFActorPlacementTA->GetPlacedActorClass())
		{
			DFActorPlacementTA->SetPlacedActorClass(TargetStructureClass);
		}

		if (DFActorPlacementTA)
		{
			GetAbilitySystemComponentFromActorInfo()->SpawnedTargetActors.Push(DFActorPlacementTA);

			// TODO: Reuse AbilityTask
			UDFAT_WaitTargetData_ReusableTA* WaitTargetDataTask = UDFAT_WaitTargetData_ReusableTA::WaitTargetDataUsingReusableTA(this, FName("WaitBuildTargetData"), EGameplayTargetingConfirmation::Type::UserConfirmed, DFActorPlacementTA);
			if (WaitTargetDataTask)
			{
				WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_BuildStructure::OnTargetDataReadyCallback);
				WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_BuildStructure::OnTargetDataCancelledCallback);
				WaitTargetDataTask->ReadyForActivation();
			}
		}
		else
		{
			DF_NETGASLOG(LogDFGAS, Error, TEXT("Target actor is nullptr."));
			CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		}
	}
}

void UGA_BuildStructure::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start. EndState: %s"), bWasCancelled ? TEXT("Cancelled") : TEXT("Confirmed"));
	GetAbilitySystemComponentFromActorInfo()->SpawnedTargetActors.Remove(DFActorPlacementTA);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BuildStructure::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	if (IsLocallyControlled())
	{
		if (TargetDataHandle.IsValid(0))
		{
			if (const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0))
			{
				if (const FHitResult* TargetHitResult = TargetData->GetHitResult())
				{
					ServerRPCSpawnTargetStructure(TargetStructureClass, TargetHitResult->Location);
				}
			}
		}
	}
}

void UGA_BuildStructure::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	BuiltStructure = nullptr;
	OnRep_BuiltStructure();
}

void UGA_BuildStructure::ServerRPCSpawnTargetStructure_Implementation(TSubclassOf<class ADFStructureBase> InTargetStructureClass, const FVector_NetQuantize SpawnLocation)
{
	DF_NETGASLOG(LogDFNET, Log, TEXT("Start"));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	BuiltStructure = GetWorld()->SpawnActor<ADFStructureBase>(InTargetStructureClass, FTransform(SpawnLocation), SpawnParams);

	ADFGameState* DFGameState = CastChecked<ADFGameState>(GetWorld()->GetGameState());
	DFGameState->SpendMoney(BuiltStructure->GetBuildCost());

	if (IsLocallyControlled())
	{
		OnRep_BuiltStructure();
	}
}

void UGA_BuildStructure::OnRep_BuiltStructure()
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	if (APawn* AvatarPawn = Cast<APawn>(CurrentActorInfo->AvatarActor))
	{
		if (IPlayerBuildModeInterface* BuildModeInterface = Cast<IPlayerBuildModeInterface>(AvatarPawn->GetController()))
		{
			BuildModeInterface->ExitBuildMode(BuiltStructure.Get());
		}
	}

	TargetStructureClass = nullptr;
	if (BuiltStructure.IsValid())
	{
		BuiltStructure = nullptr;
		K2_EndAbility();
	}
	else
	{
		K2_CancelAbility();
	}
}
