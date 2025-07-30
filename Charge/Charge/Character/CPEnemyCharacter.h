// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CPCharacterBase.h"
#include "Interface/CPFireProjectileInterface.h"
#include "Interface/CPAIEnemyInterface.h"
#include "CPEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API ACPEnemyCharacter : public ACPCharacterBase, public ICPFireProjectileInterface, public ICPAIEnemyInterface
{
	GENERATED_BODY()
	
public:
	ACPEnemyCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AController* InDamageInstigatorController, AActor* InDamageCauser, const TSubclassOf<UDamageType> InDamageTypeClass = UDamageType::StaticClass()) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	FORCEINLINE virtual UAnimInstance* GetAnimInstance() const override { return GetMesh()->GetAnimInstance(); }

	FORCEINLINE virtual APawn* GetAttackTarget() override { return AttackTarget.Get(); }
	FORCEINLINE virtual void SetAttackTarget(APawn* InAttackTarget) override { AttackTarget = InAttackTarget; }
	virtual void MulticastRPCSetDead_Implementation() override;

	virtual FVector GetProjectileSpawnLocation() const override;
	virtual FVector GetProjectileTargetLocation() const override;

	virtual void SetIgnoreMovement(bool bIgnore) override;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

// AI
public:
	FORCEINLINE class UBehaviorTree* GetBehaviorTreeAsset() { return BehaviorTreeAsset; }
	FORCEINLINE class UBlackboardData* GetBlackBoardAsset() { return BlackBoardAsset; }

protected:
	void SetBBComponentAttackTarget(APawn* InAttackTarget);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class ACPEnemyAIController> CPEnemyAIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<class UBlackboardData> BlackBoardAsset;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadWrite, Category = Combat)
	TWeakObjectPtr<APawn> AttackTarget;

// GAS
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UCPCharacterBaseAttributeSet> CharacterBaseAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UCPWeaponAttributeSet> WeaponAttributeSet;
};
