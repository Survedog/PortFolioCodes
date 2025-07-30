// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "DFGATA_Trace.generated.h"

UENUM(BlueprintType)
enum class ETargetActorCollisionFilterMethod : uint8
{
	CollisionProfile,
	CollisionChannel
};

/**
 *  Custom reusable version of AGameplayAbilityTargetActor_Trace.
 *  This target actor reuses reticle actor, avoiding duplicate spawning of it.
 *  Meant to be used with AbilityTask DFAT_WaitTargetData_ReusableTA.
 */
UCLASS(Abstract)
class DEFENSEFORCE_API ADFGATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ADFGATA_Trace();	
	
	virtual void StartTargeting(UGameplayAbility* InAbility) override;

	virtual void ConfirmTargeting() override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void CancelTargeting() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void StopTargeting();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTraceProfile(FName InProfileName) { TraceProfile = FCollisionProfileName(InProfileName); CollisionFilterMethod = ETargetActorCollisionFilterMethod::CollisionProfile; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTraceChannel(ECollisionChannel InCollisionChannel) { TraceChannel = InCollisionChannel; CollisionFilterMethod = ETargetActorCollisionFilterMethod::CollisionChannel; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCollisionFilterMethod(ETargetActorCollisionFilterMethod InCollisionFilterMethod) { CollisionFilterMethod = InCollisionFilterMethod; }

	UFUNCTION(BlueprintNativeEvent)
	void OnStartTargeting(UGameplayAbility* InAbility);

	UFUNCTION(BlueprintNativeEvent)
	void OnConfirmTargeting();

	UFUNCTION(BlueprintNativeEvent)
	void OnCancelTargeting();

	UFUNCTION(BlueprintNativeEvent)
	void OnStopTargeting();

	void OnStartTargeting_Implementation(UGameplayAbility* InAbility) {}
	void OnConfirmTargeting_Implementation() {}
	void OnCancelTargeting_Implementation() {}
	void OnStopTargeting_Implementation() {}

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) PURE_VIRTUAL(ADFGATA_Trace, return FHitResult();)

	FGameplayAbilityTargetDataHandle MakeTargetData(const FHitResult& HitResult) const;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = "Trace")
	FCollisionProfileName TraceProfile;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel;	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	ETargetActorCollisionFilterMethod CollisionFilterMethod;	

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Trace")
	FHitResult TraceHitResult;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Targeting")
	TWeakObjectPtr<AGameplayAbilityWorldReticle> ReticleActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Targeting")
	uint8 bHideReticleWhenTargetInvalid : 1;
};
