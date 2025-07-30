// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "CPProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHitDelegate, const FHitResult&, HitResult);

UCLASS(Abstract)
class CHARGE_API ACPProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPProjectileBase();
	virtual void PostInitializeComponents() override;

	FORCEINLINE void SetChargeRate(const float InChargeRate);
	FORCEINLINE void SetProjectileTargetLocation(const FVector& NewTargetLocation) { ProjectileTargetLocation = NewTargetLocation; }

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHitDelegate OnProjectileHit;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShootProjectileToTarget();

	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<class UDamageType> GetDamageType() const;
	TSubclassOf<class UDamageType> GetDamageType_Implementation() const { return UDamageType::StaticClass(); }

	UFUNCTION(BlueprintNativeEvent)
	FGameplayTagContainer GetDamageReactionAbilityTag() const;
	FGameplayTagContainer GetDamageReactionAbilityTag_Implementation() const { return FGameplayTagContainer(); }

	UFUNCTION(BlueprintNativeEvent)
	float GetDamageAmount();

	UFUNCTION(BlueprintNativeEvent)
	float GetStunValue();	

	UFUNCTION(BlueprintNativeEvent)
	void OnChargeAmountSet();

	virtual void OnChargeAmountSet_Implementation() {};

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	TObjectPtr<class UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	FLinearColor MaterialColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Movement)
	FVector ProjectileTargetLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Combat)
	float ChargeRate;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Combat)
	float DamageAmount;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Combat)
	float StunValue;

	UPROPERTY(EditAnywhere)
	bool bShootOnBeginPlay;

	UPROPERTY(EditAnywhere)
	bool bDestroyOnHit;

	UPROPERTY(EditAnywhere)
	bool bEnableDestroyTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableDestroyTimer", EditConditionHides))
	float DestroyTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS)
	TSubclassOf<class UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float MaxDamageMultiplier;

	FTimerHandle DestroyTimerHandle;
};
