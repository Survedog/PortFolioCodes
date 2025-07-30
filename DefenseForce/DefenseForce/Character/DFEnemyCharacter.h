// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/DFCharacterAnimAttackInterface.h"
#include "DFEnemyCharacter.generated.h"

UCLASS(Abstract)
class DEFENSEFORCE_API ADFEnemyCharacter : public ACharacter, public IAbilitySystemInterface, public IDFCharacterAnimAttackInterface
{
	GENERATED_BODY()

public:
	ADFEnemyCharacter();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void AttackHitCheck_Implementation() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE class AActor* GetAttackTargetActor() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FORCEINLINE float GetDamageAmount() const;

// General
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDropMoneyAmount() const { return DropMoneyAmount; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const { return bIsDead; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnDead();

	UFUNCTION()
	void OnRep_bIsDead();

protected:
	UPROPERTY(EditAnywhere, Category = "Game")
	float DropMoneyAmount;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_bIsDead, Category = "Game")
	uint8 bIsDead : 1;

//Attributes
public:
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE float GetHp() const;
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE void SetHp(float NewHp);

	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE float GetMaxHp() const;
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE void SetMaxHp(float NewMaxHp);

	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE float GetMoveSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	FORCEINLINE void SetMoveSpeed(float NewMoveSpeed);

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UDFCharacterAttributeSet> CharacterAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UDFHealthAttributeSet> HealthAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> InnateAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
};
