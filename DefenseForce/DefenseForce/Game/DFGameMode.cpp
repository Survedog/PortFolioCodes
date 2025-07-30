// Copyright 2024, Survedog. All rights reserved.


#include "Game/DFGameMode.h"
#include "Engine/AssetManager.h"

ADFGameMode::ADFGameMode()
{
}

void ADFGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for (FSoftObjectPath AssetPath : PreloadAssets)
	{
		UAssetManager::GetStreamableManager().RequestSyncLoad(AssetPath);
	}
}
