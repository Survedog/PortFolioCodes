// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "CPInputModifier_Clamp.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "CP Clamp"))
class CHARGE_API UCPInputModifier_Clamp : public UInputModifier
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings, Config)
	double Min = 0.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings, Config)
	double Max = 1.f;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
