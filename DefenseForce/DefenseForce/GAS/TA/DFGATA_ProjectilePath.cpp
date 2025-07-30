// Copyright 2024, Survedog. All rights reserved.


#include "GAS/TA/DFGATA_ProjectilePath.h"
#include "Interface/PlayerTowerControlInterface.h"
#include "Interface/DFProjectileTowerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/DFTowerBase.h"
#include "GAS/TA/Reticle/GAWorldReticle_ProjectilePath.h"
#include "DFLog.h"

ADFGATA_ProjectilePath::ADFGATA_ProjectilePath()
{
	ProjectileRadius = 0.0f;
	ProjectileGravityZ = 0.0f;
	bHideReticleWhenTargetInvalid = false;
}

void ADFGATA_ProjectilePath::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SourceActor && SourceActor->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		IPlayerTowerControlInterface* PlayerTowerControlInterface = CastChecked<IPlayerTowerControlInterface>(SourceActor);
		ADFTowerBase* ControlledTower = PlayerTowerControlInterface->GetCurrentControlledTower();
		if (ControlledTower->Implements<UDFProjectileTowerInterface>())
		{
			const FVector LaunchStartLocation = IDFProjectileTowerInterface::Execute_GetProjectileSpawnLocation(ControlledTower);
			const FVector LaunchVelocity = IDFProjectileTowerInterface::Execute_GetProjectileLaunchVelocity(ControlledTower);
			if (LaunchVelocity.IsNearlyZero())
			{
				return;
			}

			FPredictProjectilePathResult PredictResult = PerformPathPrediction(ControlledTower, LaunchStartLocation, LaunchVelocity, ProjectileRadius);
			if (AGAWorldReticle_ProjectilePath* DFProjectilePathReticle = Cast<AGAWorldReticle_ProjectilePath>(ReticleActor.Get()))
			{
				DFProjectilePathReticle->OnPredictPathResultSet(PredictResult);
			}
			else
			{
				DF_NETLOG(LogDFGAS, Warning, TEXT("Reticle class must be either GAWorldReticle_ProjectilePath or its child class."));
			}
		}
	}
}

void ADFGATA_ProjectilePath::StartTargeting(UGameplayAbility* InAbility)
{
	Super::StartTargeting(InAbility);

	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(false);
	}
}

FPredictProjectilePathResult ADFGATA_ProjectilePath::PerformPathPrediction(ADFTowerBase* InControlledTower, FVector InStartLocation, FVector InLaunchVelocity, float InProjectileRadius)
{
	FPredictProjectilePathParams PredictParams;
	PredictParams.TraceChannel = PathPredictionCollisionChannel;
	PredictParams.LaunchVelocity = InLaunchVelocity;
	PredictParams.ProjectileRadius = InProjectileRadius;
	PredictParams.OverrideGravityZ = ProjectileGravityZ;
	PredictParams.ActorsToIgnore.Add(InControlledTower);
	PredictParams.StartLocation = InStartLocation;
	PredictParams.MaxSimTime = 3.0f;
	PredictParams.SimFrequency = 10.0f;

	if (CollisionFilterMethod == ETargetActorCollisionFilterMethod::CollisionChannel)
	{
		PredictParams.bTraceWithCollision = true;
		PredictParams.bTraceWithChannel = true;
	}
	else
	{
		DF_NETLOG(LogDFGAS, Warning, TEXT("DFGATA_ProjectilePath only supports trace by collision channel."));
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictParams.DrawDebugTime = 0.1f;
	}
#endif // ENABLE_DRAW_DEBUG

	FPredictProjectilePathResult PredictPathResult;
	UGameplayStatics::PredictProjectilePath(InControlledTower->GetWorld(), PredictParams, PredictPathResult);
	return PredictPathResult;
}
