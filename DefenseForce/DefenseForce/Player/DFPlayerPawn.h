// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "DefenseForce.h"
#include "Interface/PlayerTowerControlInterface.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "DFPlayerPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTowerControlStartDelegate, ADFTowerBase*, NewControlledTower);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTowerControlEndDelegate, ADFTowerBase*, LastControlledTower);

UCLASS()
class DEFENSEFORCE_API ADFPlayerPawn : public APawn, public IAbilitySystemInterface, public IPlayerTowerControlInterface, public IDFAttackerInfoInterface
{
	GENERATED_BODY()

public:
	ADFPlayerPawn();

	FORCEINLINE class ADFPlayerController* GetDFPlayerController() const { return DFPlayerController.Get(); }

	virtual class AActor* GetAttackerActor() const override;
	virtual class UAbilitySystemComponent* GetAttackerActorASC() const override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player Control")
	TWeakObjectPtr<class ADFPlayerController> DFPlayerController;

// GAS
public:
	UFUNCTION(BlueprintCallable)
	void AbilityInputPressed(EDFAbilityInputID InputID);

	UFUNCTION(BlueprintCallable)
	void AbilityInputReleased(EDFAbilityInputID InputID);

	UFUNCTION(BlueprintCallable)
	bool TryActivateAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass, bool bAllowRemoteActivation);

	UFUNCTION(BlueprintCallable)
	void CancelAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TWeakObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TMap<EDFAbilityInputID, TSubclassOf<class UGameplayAbility>> InputAbilityMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> NonInputAbilities;

// Player Tower Control
public:	
	virtual FVector GetPlayerAimLocation() const override { return PlayerAimLocation; }
	virtual void SetPlayerAimLocation(const FVector& InPlayerAimLocation) override { PlayerAimLocation = InPlayerAimLocation; }

	UFUNCTION(BlueprintNativeEvent)
	void OnBeginCursorOverStructureCallback(AActor* TouchedActor);

	UFUNCTION(BlueprintNativeEvent)
	void OnEndCursorOverStructureCallback(AActor* TouchedActor);

public:
	UPROPERTY(BlueprintAssignable)
	FOnTowerControlStartDelegate OnTowerControlStart;

	UPROPERTY(BlueprintAssignable)
	FOnTowerControlEndDelegate OnTowerControlEnd;

protected:	
	/** Only call on server. */
	virtual void StartTowerControl(class ADFTowerBase* NewTower) override;

	/** Only call on server. */
	virtual void EndTowerControl() override;
	virtual class ADFTowerBase* GetCurrentControlledTower() const override { return CurrentControlledTower; }
	virtual class ADFStructureBase* GetCurrentStructureUnderCursor() const override { return CurrentStructureUnderCursor.Get(); }

	UFUNCTION(BlueprintNativeEvent)
	void OnTowerControlStartCallback(class ADFTowerBase* NewControlledTower);

	UFUNCTION(BlueprintNativeEvent)
	void OnTowerControlEndCallback(class ADFTowerBase* LastControlledTower);

	UFUNCTION()
	void OnRep_CurrentControlledTower(class ADFTowerBase* LastControlledTower);	

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Tower Control")
	FVector PlayerAimLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Control")
	float PlayerAimTraceLength;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Tower Control")
	TWeakObjectPtr<class ADFStructureBase> CurrentStructureUnderCursor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentControlledTower, Category = "Tower Control")
	TObjectPtr<class ADFTowerBase> CurrentControlledTower;
};
