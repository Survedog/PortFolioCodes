// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API ACPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACPGameMode();


	void HostSession(class APlayerController* HostingPlayer);

	class UCommonSession_HostSessionRequest* CreateHostingRequest() const;

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxPlayerCount;

	/** The specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId GameMapID;

	UPROPERTY(EditDefaultsOnly, Category = Memory)
	TArray<FSoftObjectPath> PreloadAssets;
};
