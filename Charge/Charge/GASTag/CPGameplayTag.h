
#pragma once

#include "GameplayTagContainer.h"

// CHARACTER
#define GASTAG_CHARACTER_ACTION_Move FGameplayTag::RequestGameplayTag(FName("Character.Action.Move"))
#define GASTAG_CHARACTER_ACTION_Aim FGameplayTag::RequestGameplayTag(FName("Character.Action.Aim"))
#define GASTAG_CHARACTER_ACTION_Charge FGameplayTag::RequestGameplayTag(FName("Character.Action.Charge"))
#define GASTAG_CHARACTER_ACTION_PrimaryAttack FGameplayTag::RequestGameplayTag(FName("Character.Action.PrimaryAttack"))

#define GASTAG_CHARACTER_COOLDOWN FGameplayTag::RequestGameplayTag(FName("Character.Cooldown"))
#define GASTAG_CHARACTER_COOLDOWN_Roll FGameplayTag::RequestGameplayTag(FName("Character.Cooldown.Roll"))
#define GASTAG_CHARACTER_COOLDOWN_FireProjectile FGameplayTag::RequestGameplayTag(FName("Character.Cooldown.FireProjectile"))

#define GASTAG_CHARACTER_COST FGameplayTag::RequestGameplayTag(FName("Character.Cost"))

#define GASTAG_CHARACTER_STATE_IsConsumingStamina FGameplayTag::RequestGameplayTag(FName("Character.State.IsConsumingStamina"))
#define GASTAG_CHARACTER_STATE_IsStunned FGameplayTag::RequestGameplayTag(FName("Character.State.IsStunned"))
#define GASTAG_CHARACTER_STATE_IsBurning FGameplayTag::RequestGameplayTag(FName("Character.State.IsBurning"))
#define GASTAG_CHARACTER_STATE_IsKnockedDown FGameplayTag::RequestGameplayTag(FName("Character.State.IsKnockedDown"))
#define GASTAG_CHARACTER_STATE_IsDead FGameplayTag::RequestGameplayTag(FName("Character.State.IsDead"))
#define GASTAG_CHARACTER_STATE_Invincible FGameplayTag::RequestGameplayTag(FName("Character.State.Invincible"))
#define GASTAG_CHARACTER_STATE_Invincible_Evasive FGameplayTag::RequestGameplayTag(FName("Character.State.Invincible.Evasive"))

#define GASTAG_CHARACTER_STAT_Stamina FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina"))
#define GASTAG_CHARACTER_STAT_DamageToApply FGameplayTag::RequestGameplayTag(FName("Character.Stat.DamageToApply"))
#define GASTAG_CHARACTER_STAT_StunGauge FGameplayTag::RequestGameplayTag(FName("Character.Stat.StunGauge"))
#define GASTAG_CHARACTER_STAT_MoveSpeed FGameplayTag::RequestGameplayTag(FName("Character.Stat.MoveSpeed"))
#define GASTAG_CHARACTER_STAT_MeeleAttackRangeMultiplier FGameplayTag::RequestGameplayTag(FName("Character.Stat.MeeleAttackRangeMultiplier"))

// Weapon
#define GASTAG_WEAPON_STAT_MaxChargeAmount FGameplayTag::RequestGameplayTag(FName("Weapon.Stat.MaxChargeAmount"))
#define GASTAG_WEAPON_STAT_ChargeSpeed FGameplayTag::RequestGameplayTag(FName("Weapon.Stat.ChargeSpeed"))
#define GASTAG_WEAPON_STAT_Damage FGameplayTag::RequestGameplayTag(FName("Weapon.Stat.Damage"))
#define GASTAG_WEAPON_STAT_StunValue FGameplayTag::RequestGameplayTag(FName("Weapon.Stat.StunValue"))

#define GASTAG_WEAPON_DamageType FGameplayTag::RequestGameplayTag(FName("Weapon.DamageType"))