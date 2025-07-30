// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CPInputModifier_Clamp.h"

FInputActionValue UCPInputModifier_Clamp::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// Don't try and clamp bools
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean, TEXT("CP Clamp modifier doesn't support boolean values.")))
	{
		const FVector CurrentVectorValue = CurrentValue.Get<FVector>();
		return FVector(FMath::Clamp(CurrentVectorValue.X, Min, Max),
					   FMath::Clamp(CurrentVectorValue.Y, Min, Max),
					   FMath::Clamp(CurrentVectorValue.Z, Min, Max));
	}
	return CurrentValue;
}
