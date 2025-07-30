// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "GameplayTagContainer.h"
#include "CPCharacterBase.generated.h"

UCLASS(Abstract)
class CHARGE_API ACPCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICPCharacterAttackInterface
{
	GENERATED_BODY()

public:
	ACPCharacterBase();

	virtual void BeginPlay() override;
	
	virtual void SetIgnoreMovement(bool bIgnore) PURE_VIRTUAL(ACPCharacterBase::SetIgnoreMovement,);

protected:	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCSetDead();

//GAS
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void ActivateDamageReactionAbility(const FGameplayEffectContextHandle& DamageEffectContext, const FGameplayTagContainer& ReactionAbilityTag);

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> NonInputAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitCharacterStatEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS)
	TSubclassOf<class UGameplayEffect> AttackDamageEffect;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS)
	//TMap<TSubclassOf<class UDamageType>, FGameplayTagContainer> DamageTypeReactionTagMap;

//Animation
public:
	FORCEINLINE virtual UAnimInstance* GetAnimInstance() const { return GetMesh()->GetAnimInstance(); }
	FORCEINLINE UAnimMontage* GetKnockedDownMontage() const { return KnockedDownMontage; }
	FORCEINLINE UAnimMontage* GetStunnedMontage() const { return StunnedMontage; }

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontage(class UAnimMontage* Montage, FName StartSectionName = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontageOnRemotes(class UAnimMontage* Montage, FName StartSectionName = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontageOnRemotes_AdjustLocation(class UAnimMontage* Montage, const FVector_NetQuantize& NewLocation, FName StartSectionName = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontageOnRemotes_AdjustRotation(class UAnimMontage* Montage, const FRotator& NewRotation, FName StartSectionName = NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerRPCPlayMontageOnRemotes_AdjustTransform(class UAnimMontage* Montage, const FTransform& NewTransform, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayMontage(class UAnimMontage* Montage, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayMontageOnRemotes(class UAnimMontage* Montage, FName StartSectionName = NAME_None);

	UFUNCTION(BlueprintCallable)
	void PlayOrJumpMontage(class UAnimMontage* Montage, FName StartSectionName = NAME_None);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<UAnimMontage> KnockedDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<UAnimMontage> StunnedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<UAnimMontage> DeadMontage;

//Combat
public:
	virtual bool IsOnSameTeamWith(APawn* OtherPawn) const;
	virtual void TriggerAttackEvent(float InChargeRate = 0.0f) const;

	FORCEINLINE virtual class UWeapon* GetCurrentWeapon() override;	
	
	FORCEINLINE class UCharacterWeaponComponent* GetCharacterWeaponComponent() { return WeaponComp; }
	void SetWeaponMesh(class UStaticMesh* NewWeaponMesh, const FTransform& NewMeshRelativeTransform = FTransform::Identity);

	virtual float TakeDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AController* InDamageInstigatorController, AActor* InDamageCauser, const TSubclassOf<UDamageType> InDamageTypeClass = UDamageType::StaticClass());

private:
	virtual float TakeDamage(float InDamageAmount, struct FDamageEvent const& InDamageEvent, class AController* InEventInstigatorController, AActor* InDamageCauser) override;

protected:
	FORCEINLINE virtual FVector GetAttackerLocation() override { return GetActorLocation(); }
	FORCEINLINE virtual FRotator GetAttackerRotation() override { return GetActorRotation(); }
	FORCEINLINE virtual FRotator GetAttackerControlRotation() override { return Controller->GetControlRotation(); }

	FORCEINLINE virtual void SetAttackerLocation(FVector NewLocation) override { SetActorLocation(NewLocation); }
	FORCEINLINE virtual void SetAttackerRotation(FRotator NewRotation) override { SetActorRotation(NewRotation); }
	FORCEINLINE virtual void SetAttackerControlRotation(FRotator NewRotation) override { Controller->SetControlRotation(NewRotation); }

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	TObjectPtr<class UCharacterWeaponComponent> WeaponComp;

//UI
public:
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MulticastRPCSpawnDamageIndicator(float InDamageAmount, AController* InDamageInstigatorController);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class ADamageIndicator> DamageIndicatorClass;
};
