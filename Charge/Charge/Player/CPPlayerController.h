// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API ACPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACPPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnRep_PlayerState() override;

// UI
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCPHUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCPHUDWidget> HUDWidget;
	
};
