// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API ADFGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADFGameMode();

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UPROPERTY(EditDefaultsOnly, Category="Memory")
	TArray<FSoftObjectPath> PreloadAssets;
};
