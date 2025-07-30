// Copyright 2024, Survedog. All rights reserved.


#include "GAS/TA/DFGATA_LineTrace.h"

ADFGATA_LineTrace::ADFGATA_LineTrace()
{
	MaxRange = 999999.0f;
	bIgnoreBlockingHits = false;
	bStartTraceFromPlayerCamera = true;
	bTraceTowardMouseAimLocation = true;
}

bool ADFGATA_LineTrace::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	if (DotToCenter >= 0)		//If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;						//Subtracting instead of adding will get the other intersection point
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);		//Cam aim point clipped to range sphere
			return true;
		}
	}
	return false;
}

void ADFGATA_LineTrace::LineTraceWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (const FHitResult& Hit : HitResults)
	{
		if (!Hit.HitObjectHandle.IsValid() || FilterHandle.FilterPassesForActor(Hit.HitObjectHandle.FetchActor()))
		{
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

void ADFGATA_LineTrace::LineTraceWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, ECollisionChannel CollisionChannel, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByChannel(HitResults, Start, End, CollisionChannel, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (const FHitResult& Hit : HitResults)
	{
		if (!Hit.HitObjectHandle.IsValid() || FilterHandle.FilterPassesForActor(Hit.HitObjectHandle.FetchActor()))
		{
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

void ADFGATA_LineTrace::SweepWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape CollisionShape, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->SweepMultiByProfile(HitResults, Start, End, Rotation, ProfileName, CollisionShape, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (const FHitResult& Hit : HitResults)
	{
		if (!Hit.HitObjectHandle.IsValid() || FilterHandle.FilterPassesForActor(Hit.HitObjectHandle.FetchActor()))
		{
			OutHitResult = Hit;
			OutHitResult.bBlockingHit = true; // treat it as a blocking hit
			return;
		}
	}
}

FHitResult ADFGATA_LineTrace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ADFGATA_Trace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(InSourceActor);
	Params.bIgnoreBlocks = bIgnoreBlockingHits;

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();// InSourceActor->GetActorLocation();
	FVector TraceEnd = FVector::Zero();

	if (PrimaryPC)
	{
		FVector PlayerViewLocation;
		FRotator PlayerViewRotation;
		PrimaryPC->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

		if (bStartTraceFromPlayerCamera)
		{
			TraceStart = PlayerViewLocation;
		}

		if (bTraceTowardMouseAimLocation)
		{
			FVector MouseWorldLocation, MouseWorldDirection;
			PrimaryPC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

			FVector TraceDirection = (MouseWorldLocation - TraceStart).GetSafeNormal();
			TraceEnd = TraceDirection.IsNearlyZero() ? PlayerViewLocation + PlayerViewRotation.Vector() * MaxRange : TraceStart + TraceDirection * MaxRange;
		}
		else
		{
			TraceEnd = PlayerViewLocation + PlayerViewRotation.Vector() * MaxRange;
		}
	}

	// ------------------------------------------------------

	FHitResult ReturnHitResult;
	//Use a line trace initially to see where the player is actually pointing
	if (CollisionFilterMethod == ETargetActorCollisionFilterMethod::CollisionProfile)
	{
		LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	}
	else
	{
		LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceChannel, Params);
	}

	//Default to end of trace line if we don't hit anything.
	if (!ReturnHitResult.bBlockingHit)
	{
		ReturnHitResult.Location = TraceEnd;
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, ReturnHitResult.Location, FColor::Green, false);
		DrawDebugSphere(GetWorld(), ReturnHitResult.Location, 16, 10, FColor::Green, false);
	}
#endif // ENABLE_DRAW_DEBUG

	// Reset the trace start so the target data uses the correct origin
	ReturnHitResult.TraceStart = StartLocation.GetTargetingTransform().GetLocation();

	return ReturnHitResult;
}