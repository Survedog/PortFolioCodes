// Copyright 2024, Survedog. All rights reserved.

#pragma once

#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define LOG_UILOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwningPlayer()->GetLocalRole()))
#define LOG_UIREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwningPlayer()->GetRemoteRole()))
#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define LOG_ATTLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetActorInfo()->AvatarActor.Get()->GetLocalRole()))
#define LOG_ATTREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetActorInfo()->AvatarActor.Get()->GetRemoteRole()))
#define LOG_GASLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetCurrentActorInfo()->AvatarActor.Get()->GetLocalRole()))
#define LOG_GASREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetCurrentActorInfo()->AvatarActor.Get()->GetRemoteRole()))
#define LOG_CUSTOMLOCALROLEINFO(Actor) *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), (Actor)->GetLocalRole()))
#define LOG_CUSTOMREMOTEROLEINFO(Actor) *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), (Actor)->GetRemoteRole()))

#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID.operator int32()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_UINETMODEINFO ((GetOwningPlayer()->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID.operator int32()) : ((GetOwningPlayer()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_ATTNETMODEINFO ((GetActorInfo()->OwnerActor.Get()->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID.operator int32()) : ((GetActorInfo()->OwnerActor.Get()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_GASNETMODEINFO ((GetCurrentActorInfo()->OwnerActor.Get()->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID.operator int32()) : ((GetCurrentActorInfo()->OwnerActor.Get()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_CUSTOMNETMODEINFO(Actor) (((Actor)->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID.operator int32()) : (((Actor)->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define DF_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %s"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NETMODEINFO, LOG_LOCALROLEINFO, LOG_REMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETUILOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_UINETMODEINFO, LOG_UILOCALROLEINFO, LOG_UIREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETSUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NETMODEINFO, LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETATTLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_ATTNETMODEINFO, LOG_ATTLOCALROLEINFO, LOG_ATTREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETGASLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_GASNETMODEINFO, LOG_GASLOCALROLEINFO, LOG_GASREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define DF_NETCUSTOMLOG(Actor, LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_CUSTOMNETMODEINFO(Actor), LOG_CUSTOMLOCALROLEINFO(Actor), LOG_CUSTOMREMOTEROLEINFO(Actor), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogDF, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDFAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDFUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDFNET, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDFGAS, Log, All);