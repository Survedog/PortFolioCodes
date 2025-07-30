// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/TA/DFGATA_Trace.h"
#include "DFGATA_LineTrace.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFGATA_LineTrace : public ADFGATA_Trace
{
	GENERATED_BODY()
	
public:
	ADFGATA_LineTrace();

	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	/** Traces as normal, but will manually filter all hit actors */
	static void LineTraceWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);
	static void LineTraceWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, ECollisionChannel CollisionChannel, const FCollisionQueryParams Params);

	/** Sweeps as normal, but will manually filter all hit actors */
	static void SweepWithFilter(FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape CollisionShape, FName ProfileName, const FCollisionQueryParams Params);

	FORCEINLINE float GetMaxRange() const { return MaxRange; }
	FORCEINLINE void SetMaxRange(float InMaxRange) { MaxRange = InMaxRange; }

	FORCEINLINE bool ShouldIgnoreBlockingHits() const { return bIgnoreBlockingHits; }
	FORCEINLINE void SetIgnoreBlockingHits(bool bInIgnoreBlockingHits) { bIgnoreBlockingHits = bInIgnoreBlockingHits; }

	FORCEINLINE bool ShouldStartTraceFromPlayerCamera() const { return bStartTraceFromPlayerCamera; }
	FORCEINLINE void SetStartTraceFromPlayerCamera(bool bInStartTraceFromPlayerCamera) { bStartTraceFromPlayerCamera = bInStartTraceFromPlayerCamera; }

	FORCEINLINE bool ShouldTraceTowardMouseLocation() const { return bTraceTowardMouseAimLocation; }
	FORCEINLINE void SetTraceTowardMouseAimLocation(bool bInTraceTowardMouseAimLocation) { bTraceTowardMouseAimLocation = bInTraceTowardMouseAimLocation; }

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	float MaxRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bIgnoreBlockingHits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bStartTraceFromPlayerCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bTraceTowardMouseAimLocation;
};
