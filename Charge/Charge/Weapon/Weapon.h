// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackComboProceedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UENUM(BlueprintType)
enum class EAttackEndTiming : uint8
{
	OnMontageEnded,
	OnProjectileSpawned,
	Custom
};

UENUM(BlueprintType)
enum class EProjectileSpawnTiming : uint8
{
	OnAttackStarted,
	OnAnimNotify,
	Custom,
	None
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CHARGE_API UWeapon : public UObject
{
	GENERATED_BODY()
	
public:
	UWeapon();

	FORCEINLINE class ACPCharacterBase* GetCPCharacterOwner() const { return CPCharacterOwner.Get(); }
	class AActor* GetActorOwner() const;

	UFUNCTION(BlueprintNativeEvent)
	bool CanDealDamageTo(APawn* OtherPawn) const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttackComboProceedDelegate OnAttackComboProceed;

	UPROPERTY(BlueprintAssignable)
	FOnAttackEndDelegate OnAttackEnd;

	UPROPERTY(BlueprintAssignable)
	FOnAttackStartDelegate OnAttackStart;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Character)
	TWeakObjectPtr<class ACPCharacterBase> CPCharacterOwner;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Character)
	TWeakObjectPtr<class UCharacterWeaponComponent> OwnerWeaponComp;

// Networking
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Network)
	uint8 bIsProjectileReplicated : 1;

// Combat
public:
	UFUNCTION(BlueprintNativeEvent)
	void StartComboAttack(float InChargeRate);

	UFUNCTION(BlueprintCallable)
	void ProceedComboAttack();	

	UFUNCTION(BlueprintNativeEvent)
	void EndPrimaryAttack();	

	UFUNCTION(BlueprintCallable)
	void DealDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AActor* InDamageCauser, TSubclassOf<UDamageType> InDamageTypeClass);

	//UFUNCTION(BlueprintNativeEvent)
	//void ApplyAttackAdditionalEffects(float InDamageAmount, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AActor* InDamageCauser, TSubclassOf<UDamageType> InDamageTypeClass);

	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
	FORCEINLINE bool IsWaitingForNextAttack() const { return bIsWaitingForNextAttack; }
	FORCEINLINE bool HasAttackInput() const { return bHasAttackInput; }
	FORCEINLINE void SetIsWaitingForNextAttack(bool bInIsWaitingForNextAttack) { bIsWaitingForNextAttack = bInIsWaitingForNextAttack; }
	FORCEINLINE void SetHasAttackInput(bool bInHasAttackInput) { bHasAttackInput = bInHasAttackInput; }

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetCurrentAttackComboCount() const { return CurrentComboCount; }
	FORCEINLINE int32 GetMaxPrimaryAttackComboCount() const { return MaxPrimaryAttackComboCount; }
	FORCEINLINE EProjectileSpawnTiming GetProjectileSpawnTiming() const { return ProjectileSpawnTiming; }

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<ACPProjectileBase> GetProjectileClass() const;

protected:
	friend class UAnimNotify_SpawnProjectile;

	UFUNCTION(BlueprintNativeEvent)
	void PrimaryAttack(int32 InComboCount = 1);

	UFUNCTION(BlueprintNativeEvent)
	UAnimMontage* GetPrimaryAttackMontage() const;

	void OnAttackMontageBlendOutCallback(UAnimMontage* Montage, bool bInterrupted);
	void OnAttackMontageEndedCallback(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontageOnAll(UAnimMontage* InAttackMontage, int32 InComboCount = 1);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayAttackMontageOnRemotes(UAnimMontage* InAttackMontage, int32 InComboCount = 1);

	void PlayAttackMontage(UAnimMontage* InAttackMontage, int32 InComboCount = 1);	

	UFUNCTION(BlueprintCallable)
	void SpawnProjectileOnAll();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSpawnProjectileOnRemotes(TSubclassOf<class ACPProjectileBase> InProjectileClass, const FVector_NetQuantize InTargetLocation, float InChargeRate);

	UFUNCTION(BlueprintNativeEvent)
	class ACPProjectileBase* SpawnProjectile(TSubclassOf<class ACPProjectileBase> InProjectileClass, const FVector_NetQuantize InTargetLocation, float InChargeRate);

protected:
	friend class UCharacterWeaponComponent;

	UFUNCTION(BlueprintNativeEvent)
	void OnWeaponObtained(class UCharacterWeaponComponent* NewWeaponComp);

	UFUNCTION(BlueprintNativeEvent)
	void OnWeaponDropped(class UCharacterWeaponComponent* OldWeaponComp);

	UFUNCTION(BlueprintNativeEvent)
	void OnWeaponEquipped();

	UFUNCTION(BlueprintNativeEvent)
	void OnWeaponUnequipped();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	TObjectPtr<UAnimMontage> PrimaryAttackMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	float CurrentAttackChargeRate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Combat)
	uint8 bIsAttacking : 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	uint8 bIsWaitingForNextAttack : 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	uint8 bHasAttackInput : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	uint8 bStopMovementOnAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	EProjectileSpawnTiming ProjectileSpawnTiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	EAttackEndTiming AttackEndTiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (ClampMin = 0, ClampMax = 127))
	int32 MaxPrimaryAttackComboCount;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Combat)
	int32 CurrentComboCount;

// UI
public:
	FORCEINLINE TSoftObjectPtr<class UTexture2D> GetWeaponThumbnailTexture() const { return WeaponThumbnailTexture; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSoftObjectPtr<class UTexture2D> WeaponThumbnailTexture;

// Mesh
public:
	FORCEINLINE TSoftObjectPtr<class UStaticMesh> GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE const FTransform& GetMeshRelativeTransform() const { return MeshRelativeTransform; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	TSoftObjectPtr<class UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	FTransform MeshRelativeTransform;

// Stat
public:
	FORCEINLINE float GetBaseChargeSpeed() const { return BaseChargeSpeed; }
	FORCEINLINE float GetBaseMaxChargeAmount() const { return BaseMaxChargeAmount; }
	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }
	FORCEINLINE float GetBaseStunValue() const { return BaseStunValue; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	float GetAttackDamageMultiplier() const;

	UFUNCTION(BlueprintNativeEvent)
	float GetStunValueMultiplier() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float BaseChargeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float BaseMaxChargeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float BaseStunValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> MultiplyDamageAmountEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> MultiplyStunValueEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	FActiveGameplayEffectHandle ActiveMultiplyDamageAmountEffectHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	FActiveGameplayEffectHandle ActiveMultiplyStunValueEffectHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float MaxAttackDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, meta = (ClampMin = 0))
	float MaxStunValueMultiplier;
};
