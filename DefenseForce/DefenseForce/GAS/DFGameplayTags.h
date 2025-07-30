// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#define GASTAG_Structure_Action_Attack FGameplayTag::RequestGameplayTag(TEXT("Structure.Action.Attack"))
#define GASTAG_Structure_Action_Attack_Cooldown FGameplayTag::RequestGameplayTag(TEXT("Structure.Action.Attack.Cooldown"))
#define GASTAG_Player_Action_BuildStructure FGameplayTag::RequestGameplayTag(TEXT("Player.Action.BuildStructure"))
#define GASTAG_Character_Action_Attack FGameplayTag::RequestGameplayTag(TEXT("Character.Action.Attack"))
#define GASTAG_Character_State_Dead FGameplayTag::RequestGameplayTag(TEXT("Character.State.Dead"))
#define GASTAG_Attribute_Health_DamageToApply FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health.DamageToApply"))