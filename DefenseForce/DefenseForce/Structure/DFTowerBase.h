// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structure/DFStructureBase.h"
#include "DFTowerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DEFENSEFORCE_API ADFTowerBase : public ADFStructureBase
{
	GENERATED_BODY()
	
public:
	ADFTowerBase();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsBeingControlled() const { return bIsBeingControlled; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsLocallyControlled() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnControlStart(class ADFPlayerPawn* NewPlayerPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnControlEnd(class ADFPlayerPawn* LastPlayerPawn);

	UFUNCTION(BlueprintCallable)
	class AGameplayAbilityTargetActor* GetAttackTargetActor();

	UFUNCTION(BlueprintNativeEvent)
	void InitializeTargetActor(class AGameplayAbilityTargetActor* InTargetActor);

	FORCEINLINE TSubclassOf<class AGameplayAbilityTargetActor> GetTargetActorClass() const { return TargetActorClass; }
	FORCEINLINE TSubclassOf<class AGameplayAbilityWorldReticle> GetReticleClass() const { return ReticleClass; }
	FORCEINLINE bool ShouldConfirmTargetOnInputPressed() const { return bConfirmTargetOnInputPressed; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UGASWidgetComponent> CooldownWidgetComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Player Control")
	TWeakObjectPtr<class ADFPlayerPawn> ControllingPlayerPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TSubclassOf<class AGameplayAbilityTargetActor> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TSubclassOf<class AGameplayAbilityWorldReticle> ReticleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	uint8 bConfirmTargetOnInputPressed : 1;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Player Control")
	uint8 bIsBeingControlled : 1;
};
