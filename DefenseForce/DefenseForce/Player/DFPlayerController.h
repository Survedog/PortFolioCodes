// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interface/PlayerTowerControlInterface.h"
#include "Interface/PlayerBuildModeInterface.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "DFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFPlayerController : public APlayerController, public IPlayerTowerControlInterface, public IPlayerBuildModeInterface, public IDFAttackerInfoInterface
{
	GENERATED_BODY()
	
public:
	ADFPlayerController();

	virtual class AActor* GetAttackerActor() const override;

	virtual class UAbilitySystemComponent* GetAttackerActorASC() const override;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void AcknowledgePossession(class APawn* P) override;

/* PlayerTowerControlInterface */
protected:
	/** Only call on server. */
	virtual void StartTowerControl(class ADFTowerBase* NewTower) override;	

	/** Only call on server. */
	virtual void EndTowerControl() override;

	virtual class ADFTowerBase* GetCurrentControlledTower() const override;
	virtual class ADFStructureBase* GetCurrentStructureUnderCursor() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTowerControlStartCallback(class ADFTowerBase* NewControlledTower);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTowerControlEndCallback(class ADFTowerBase* OldControlledTower);

	virtual FVector GetPlayerAimLocation() const override;
	virtual void SetPlayerAimLocation(const FVector& InPlayerAimLocation) override;

protected:
	/* PlayerBuildModeInterface */
	virtual void EnterBuildMode(TSubclassOf<class ADFStructureBase> InTargetStructureClass) override;
	virtual void ExitBuildMode(class ADFStructureBase* InBuiltStructure) override;
	
	bool CanBuildStructure(TSubclassOf<class ADFStructureBase> InTargetStructureClass) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnEnterBuildMode(TSubclassOf<class ADFStructureBase> InTargetStructureClass);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExitBuildMode(class ADFStructureBase* InBuiltStructure);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player")
	TObjectPtr<class ADFPlayerPawn> DFPlayerPawn;
};
