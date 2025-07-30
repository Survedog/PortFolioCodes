// Copyright 2024, Survedog. All rights reserved.


#include "GAS/TA/DFGATA_ActorPlacement.h"
#include "GAS/TA/Reticle/GAWorldReticle_ActorVisualization.h"
#include "DFLog.h"

ADFGATA_ActorPlacement::ADFGATA_ActorPlacement()
{
	static ConstructorHelpers::FClassFinder<AGAWorldReticle_ActorVisualization> ReticleClassRef(TEXT("/Game/DefenseForce/Blueprint/GAS/TA/ReticleActor/BP_GAWorldReticle_ActorVisualization.BP_GAWorldReticle_ActorVisualization_C"));
	if (ReticleClassRef.Class)
	{
		ReticleClass = ReticleClassRef.Class;
	}
}

void ADFGATA_ActorPlacement::StartTargeting(UGameplayAbility* InAbility)
{
	Super::StartTargeting(InAbility);

	if (ReticleActor.IsValid())
	{
		if (AGAWorldReticle_ActorVisualization* ActorVisualizationReticle = Cast<AGAWorldReticle_ActorVisualization>(ReticleActor))
		{			
			ActorVisualizationReticle->InitializeVisualizationInformation(PlacedActorClass, PlacedActorMaterial);
		}
		else
		{
			DF_NETLOG(LogDFGAS, Warning, TEXT("Reticle actor is not an instance of class 'AGameplayAbilityWorldReticle_ActorVisualization'"));
		}
	}
}

void ADFGATA_ActorPlacement::ConfirmTargeting()
{
	if (TraceHitResult.bBlockingHit)
	{
		Super::ConfirmTargeting();
	}
	else
	{
		OnTargetConfirmRejected.Broadcast();
	}
}

void ADFGATA_ActorPlacement::ConfirmTargetingAndContinue()
{
	if (TraceHitResult.bBlockingHit)
	{
		Super::ConfirmTargetingAndContinue();
	}
	else
	{
		OnTargetConfirmRejected.Broadcast();
	}
}
