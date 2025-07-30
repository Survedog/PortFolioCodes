// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "DefenseForce.h"
#include "DFStructureBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructureDestructedDelegate);

UCLASS()
class DEFENSEFORCE_API ADFStructureBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ADFStructureBase();
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

	FORCEINLINE float GetBuildCost() const { return BuildCost; }
	FORCEINLINE void SetBuildCost(float NewBuildCost) { BuildCost = NewBuildCost; }

	UFUNCTION(BlueprintCallable)
	void AbilityInputPressed(EDFAbilityInputID InputID);

	UFUNCTION(BlueprintCallable)
	void AbilityInputReleased(EDFAbilityInputID InputID);

	UFUNCTION(BlueprintCallable)
	bool TryActivateAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass, bool bAllowRemoteActivation);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload);

	UFUNCTION(BlueprintCallable)
	void CancelAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass);

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

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnDestructedCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FOnStructureDestructedDelegate OnStructureDestructed;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootSceneComponent;		

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UDFHealthAttributeSet> HealthAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> NonInputAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TMap<EDFAbilityInputID, TSubclassOf<class UGameplayAbility>> InputAbilityMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Build")
	float BuildCost;	
};
