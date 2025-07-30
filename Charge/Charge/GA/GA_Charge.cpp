// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Charge.h"
#include "Character/CPPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CPChargeAttributeSet.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

UGA_Charge::UGA_Charge() : ChargeEffect(nullptr), InitChargeAmountEffect(nullptr)
{
    bServerRespectsRemoteAbilityCancellation = true;

    static ConstructorHelpers::FClassFinder<UGameplayEffect> ChargeEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_ChargeWeapon.BPGE_ChargeWeapon_C"));
    if (ChargeEffectClassRef.Class)
    {
        ChargeEffect = ChargeEffectClassRef.Class;
    }

    static ConstructorHelpers::FClassFinder<UGameplayEffect> InitChargeAmountEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_InitChargeAmount.BPGE_InitChargeAmount_C"));
    if (InitChargeAmountEffectClassRef.Class)
    {
        InitChargeAmountEffect = InitChargeAmountEffectClassRef.Class;
    }
}

bool UGA_Charge::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
    return AvatarCPCharacter->GetCurrentWeapon() != nullptr;
}

void UGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    CommitAbility(Handle, ActorInfo, ActivationInfo);

    ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    FGameplayEffectSpecHandle ChargeEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ChargeEffect);
    if (ChargeEffectSpecHandle.IsValid())
    {
        ActiveChargeEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(ChargeEffectSpecHandle);
    }
}

void UGA_Charge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));

    ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());
    UWeapon* Weapon = AvatarCPCharacter->GetCurrentWeapon();
    if (Weapon)
    {
        Weapon->OnAttackEnd.RemoveDynamic(this, &UGA_Charge::K2_EndAbility);
    }

    UAbilitySystemComponent* ASC = AvatarCPCharacter->GetAbilitySystemComponent();
    if (ActiveChargeEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(ActiveChargeEffectHandle);
    }

    FGameplayEffectSpecHandle InitChargeEffectSpecHandle = MakeOutgoingGameplayEffectSpec(InitChargeAmountEffect);
    if (InitChargeEffectSpecHandle.IsValid())
    {
        ASC->BP_ApplyGameplayEffectSpecToSelf(InitChargeEffectSpecHandle);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Charge::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    CP_NETGASLOG(LogCPGAS, Log, TEXT("Start"));
    // Do not call Super function.

    ACPPlayerCharacter* AvatarCPCharacter = CastChecked<ACPPlayerCharacter>(ActorInfo->AvatarActor.Get());  
    UWeapon* Weapon = AvatarCPCharacter->GetCurrentWeapon();
    if (Weapon)
    {
        Weapon->OnAttackEnd.AddUniqueDynamic(this, &UGA_Charge::K2_EndAbility);
        UAbilitySystemComponent* ASC = AvatarCPCharacter->GetAbilitySystemComponent();
        const UCPChargeAttributeSet* ChargeAttributeSet = ASC->GetSetChecked<UCPChargeAttributeSet>();
        const float ChargeRate = ChargeAttributeSet->GetChargeAmount() / ChargeAttributeSet->GetMaxChargeAmount();
        AvatarCPCharacter->TriggerAttackEvent(ChargeRate);
    }
    else
    {
        bool bReplicateEndAbility = false;
        bool bWasCancelled = false;
        EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    }
}