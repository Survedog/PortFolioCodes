// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/CPGameMode.h"
#include "CommonGameInstance.h"
#include "CommonSessionSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/AssetManager.h"

ACPGameMode::ACPGameMode() : MaxPlayerCount(2)
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Charge.CPPlayerCharacter"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<AController> PlayerControllerClassRef(TEXT("/Script/Charge.CPPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

void ACPGameMode::HostSession(APlayerController* HostingPlayer)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UCommonSession_HostSessionRequest* HostRequest = CreateHostingRequest();
		UCommonSessionSubsystem* CommonSessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
		if (HostRequest && CommonSessionSubsystem)
		{
			CommonSessionSubsystem->HostSession(HostingPlayer, HostRequest);
		}
	}
}

UCommonSession_HostSessionRequest* ACPGameMode::CreateHostingRequest() const
{
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->OnlineMode = ECommonSessionOnlineMode::Online;
	Result->bUseLobbies = false;
	Result->MapID = GameMapID;
	Result->MaxPlayerCount = MaxPlayerCount;

	return Result;
}

void ACPGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for (FSoftObjectPath AssetPath : PreloadAssets)
	{
		UAssetManager::GetStreamableManager().RequestSyncLoad(AssetPath);
	}
}
