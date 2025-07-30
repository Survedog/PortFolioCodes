// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "DFProjectileBase.generated.h"

UCLASS()
class DEFENSEFORCE_API ADFProjectileBase : public AActor, public IDFAttackerInfoInterface
{
	GENERATED_BODY()
	
public:	
	ADFProjectileBase();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float GetDamageAmount() const;

	virtual class AActor* GetAttackerActor() const override;

	virtual class UAbilitySystemComponent* GetAttackerActorASC() const override;

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE float ApplyPointDamage(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo, TSubclassOf<UDamageType> DamageTypeClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
	FORCEINLINE bool ApplyRadialDamage(const UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, bool bDoFullDamage = false, ECollisionChannel DamagePreventionChannel = ECC_Visibility);	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
};
