// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CPInteractiveObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCPInteractiveObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHARGE_API ICPInteractiveObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnInteract"))
	void OnInteract(class ACPPlayerCharacter* InteractedCharacter);

	virtual void OnInteract_Implementation(class ACPPlayerCharacter* InteractedCharacter) = 0;
	virtual class UStaticMeshComponent* GetMeshComponent() const = 0;
	virtual void HighlightObjectOutline();
	virtual void UnhighlightObjectOutline();
};
