// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CPCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UCPCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UCPCharacterControlData();

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FVector SocketOffset;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	bool bDoCollisionTest;

	UPROPERTY(EditAnywhere, Category = Pawn)
	bool bUseControllerRotationYaw;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	bool bOrientRotationToMovement;
};
