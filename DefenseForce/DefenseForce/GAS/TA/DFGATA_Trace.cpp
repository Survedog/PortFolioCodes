// Copyright 2024, Survedog. All rights reserved.


#include "GAS/TA/DFGATA_Trace.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemLog.h"
#include "GAS/TA/Reticle/DFGAWorldReticle.h" 
#include "Interface/PlayerTowerControlInterface.h"
#include "DFLog.h"

ADFGATA_Trace::ADFGATA_Trace()
{
	bDestroyOnConfirmation = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	
	CollisionFilterMethod = ETargetActorCollisionFilterMethod::CollisionProfile;
	bHideReticleWhenTargetInvalid = true;
}

void ADFGATA_Trace::StartTargeting(UGameplayAbility* InAbility)
{
	Super::StartTargeting(InAbility);
	DF_NETLOG(LogDFGAS, Log, TEXT("start"));

	SetActorTickEnabled(true);
	SourceActor = InAbility->GetCurrentActorInfo()->AvatarActor.Get();
		
	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = nullptr;
		if (ReticleActor.IsValid() && ReticleActor->GetClass() == ReticleClass)
		{
			SpawnedReticleActor = ReticleActor.Get();
		}
		else
		{
			if (ReticleActor.IsValid())
			{
				ReticleActor->Destroy();
			}
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		}

		if (SpawnedReticleActor)
		{
			SpawnedReticleActor->InitializeReticle(this, PrimaryPC, ReticleParams); // TODO: Check if it's okay to initialize an reused reticle actor again.
			ReticleActor = SpawnedReticleActor;

			// This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
			// (In a client controlled player, this would only run on the client and therefor never replicate. If it runs
			// on a listen server, the reticle actor may replicate. We want consistancy between client/listen server players.
			// Just saying 'make the reticle actor non replicated' isnt a good answer, since we want to mix and match reticle
			// actors and there may be other targeting types that want to replicate the same reticle actor class).
			if (!ShouldProduceTargetDataOnServer)
			{
				SpawnedReticleActor->SetReplicates(false);
			}
		}
	}

	OnStartTargeting(InAbility);
}

void ADFGATA_Trace::ConfirmTargeting()
{
	Super::ConfirmTargeting();

	SetActorTickEnabled(false);
	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(true);
	}
}

void ADFGATA_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	DF_NETLOG(LogDFGAS, Log, TEXT("start"));

	if (SourceActor && IsConfirmTargetingAllowed())
	{
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(TraceHitResult);
		TargetDataReadyDelegate.Broadcast(Handle);
		OnConfirmTargeting();
	}
}

void ADFGATA_Trace::CancelTargeting()
{
	DF_NETLOG(LogDFGAS, Log, TEXT("start"));
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	SetActorTickEnabled(false);

	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(true);
	}

	OnCancelTargeting();
}

void ADFGATA_Trace::BeginPlay()
{
	Super::BeginPlay();

	DF_NETLOG(LogDFGAS, Log, TEXT("start"));
	SetActorTickEnabled(false);
}

void ADFGATA_Trace::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DF_NETLOG(LogDFGAS, Log, TEXT("start"));
	if (ReticleActor.IsValid())
	{
		ReticleActor->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void ADFGATA_Trace::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// very temp - do a mostly hardcoded trace from the source actor
	if (SourceActor && SourceActor->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		TraceHitResult = PerformTrace(SourceActor);
		FVector EndPoint = TraceHitResult.Component.IsValid() ? TraceHitResult.ImpactPoint : TraceHitResult.TraceEnd;

		if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
		{
			LocalReticleActor->SetIsTargetValid(TraceHitResult.bBlockingHit);
			LocalReticleActor->SetIsTargetAnActor(TraceHitResult.GetActor() != nullptr);

			if (bHideReticleWhenTargetInvalid)
			{
				LocalReticleActor->SetActorHiddenInGame(!TraceHitResult.bBlockingHit);
			}

			if (TraceHitResult.bBlockingHit)
			{
				if (ADFGAWorldReticle* DFReticleActor = Cast<ADFGAWorldReticle>(LocalReticleActor))
				{
					DFReticleActor->OnTraceResultSet(TraceHitResult);
				}
				else
				{
					LocalReticleActor->SetActorLocation(TraceHitResult.Location);
				}
			}
		}
		
		if (IPlayerTowerControlInterface* PlayerTowerControlInterface = Cast<IPlayerTowerControlInterface>(SourceActor))
		{
			PlayerTowerControlInterface->SetPlayerAimLocation(EndPoint);
		}

		SetActorLocationAndRotation(EndPoint, SourceActor->GetActorRotation());
	}
}

void ADFGATA_Trace::StopTargeting()
{
	DF_NETLOG(LogDFGAS, Log, TEXT("start"));
	SetActorTickEnabled(false);

	if (ReticleActor.IsValid())
	{
		ReticleActor->SetActorHiddenInGame(true);
	}

	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();
	
	if (GenericDelegateBoundASC)
	{
		GenericDelegateBoundASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
		GenericDelegateBoundASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);
		GenericDelegateBoundASC = nullptr;
	}

	OnStopTargeting();
}

FGameplayAbilityTargetDataHandle ADFGATA_Trace::MakeTargetData(const FHitResult& HitResult) const
{
	/** Note: This will be cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
	return StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
}
