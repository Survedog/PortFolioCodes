// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Sprint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Attribute/CPStaminaAttributeSet.h"
#include "GASTag/CPGameplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Charge.h"

UGA_Sprint::UGA_Sprint() : SprintSpeedMultiplier(2.0f), StaminaCost(1.0f)
{
	bServerRespectsRemoteAbilityCancellation = true;

	static ConstructorHelpers::FClassFinder<UGameplayEffect> SprintEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_Sprint.BPGE_Sprint_C"));
	if (SprintEffectClassRef.Class)
	{
		SprintEffect = SprintEffectClassRef.Class;
	}
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* AvatarCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	const bool bIsFalling = AvatarCharacter->GetCharacterMovement()->IsFalling();
	return !bIsFalling;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* AvatarCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	const UCPStaminaAttributeSet* StaminaAttributeSet = ASC->GetSetChecked<UCPStaminaAttributeSet>();
	FGameplayEffectSpecHandle SprintEffectSpec = MakeOutgoingGameplayEffectSpec(SprintEffect);
	SprintEffectSpec.Data->SetSetByCallerMagnitude(GASTAG_CHARACTER_STAT_MoveSpeed, SprintSpeedMultiplier);
	ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SprintEffectSpec);

	StaminaAttributeSet->OnOutOfStamina.AddDynamic(this, &UGameplayAbility::K2_CancelAbility);
	ASC->RegisterGameplayTagEvent(GASTAG_CHARACTER_ACTION_Move, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_Sprint::MoveTagEventCallback);

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	ACharacter* AvatarCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (ActiveEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveEffectHandle);
	}

	if (ActiveCostEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveCostEffectHandle);
	}

	const UCPStaminaAttributeSet* StaminaAttributeSet = ASC->GetSetChecked<UCPStaminaAttributeSet>();
	StaminaAttributeSet->OnOutOfStamina.RemoveDynamic(this, &UGameplayAbility::K2_CancelAbility);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
	// Do not call super function.
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Do not call super function.
	ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	ApplyCost_SaveCostEffectHandle(Handle, ActorInfo, ActivationInfo);
}

// Non-const version of ApplyCost. It is not overriden from parent.
// Saves active handle of cost effect.
void UGA_Sprint::ApplyCost_SaveCostEffectHandle(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Do not call super function.
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass());
		ActiveCostEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
	}
}

void UGA_Sprint::MoveTagEventCallback(FGameplayTag Tag, int32 NewTagCount)
{
	if (NewTagCount == 0)
	{
		K2_CancelAbility();
	}
}
