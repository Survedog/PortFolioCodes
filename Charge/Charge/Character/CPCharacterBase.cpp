// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CPCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Physics/CPCollision.h"
#include "GASTag/CPGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "UI/DamageIndicator.h"
#include "Engine/DamageEvents.h"
#include "Charge.h"

ACPCharacterBase::ACPCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/CoreUObject.Class'/Script/Charge.CPCharacterAnimInstance'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FClassFinder<AActor> DamageIndicatorClassRef(TEXT("/Game/Charge/Blueprint/UI/BP_DamageIndicator.BP_DamageIndicator_C"));
	if (DamageIndicatorClassRef.Class)
	{
		DamageIndicatorClass = DamageIndicatorClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UGameplayEffect> AttackDamageEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_AttackDamage.BPGE_AttackDamage_C"));
	if (AttackDamageEffectClassRef.Class)
	{
		AttackDamageEffect = AttackDamageEffectClassRef.Class;
	}

	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	// Mesh
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionProfileName(CPROFILE_CHARACTERMESH);

	// Combat
	WeaponComp = CreateDefaultSubobject<UCharacterWeaponComponent>(TEXT("WeaponComponent"));
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	WeaponMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Owner;
}

void ACPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACPCharacterBase::MulticastRPCSetDead_Implementation()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	SetIgnoreMovement(true);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_BLOCKONLYWORLD);
	GetMesh()->SetCollisionProfileName(CPROFILE_BLOCKONLYWORLD);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG_CHARACTER_STATE_IsDead);
	}
}

UAbilitySystemComponent* ACPCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ACPCharacterBase::ActivateDamageReactionAbility(const FGameplayEffectContextHandle& DamageEffectContext, const FGameplayTagContainer& ReactionAbilityTags)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		FGameplayEventData Payload;
		Payload.ContextHandle = DamageEffectContext;
		for (FGameplayTag Tag : ReactionAbilityTags)
		{
			ASC->HandleGameplayEvent(Tag, &Payload);
		}
	}
}

void ACPCharacterBase::ServerRPCPlayMontage_Implementation(UAnimMontage* Montage, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	MulticastRPCPlayMontage(Montage);
}

void ACPCharacterBase::ServerRPCPlayMontageOnRemotes_Implementation(UAnimMontage* Montage, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	MulticastRPCPlayMontageOnRemotes(Montage, StartSectionName);
}

void ACPCharacterBase::ServerRPCPlayMontageOnRemotes_AdjustLocation_Implementation(UAnimMontage* Montage, const FVector_NetQuantize& NewLocation, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	SetActorLocation(NewLocation);
	ServerRPCPlayMontageOnRemotes_Implementation(Montage, StartSectionName);
}

void ACPCharacterBase::ServerRPCPlayMontageOnRemotes_AdjustRotation_Implementation(UAnimMontage* Montage, const FRotator& NewRotation, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	SetActorRotation(NewRotation);
	ServerRPCPlayMontageOnRemotes_Implementation(Montage, StartSectionName);
}

void ACPCharacterBase::ServerRPCPlayMontageOnRemotes_AdjustTransform_Implementation(UAnimMontage* Montage, const FTransform& NewTransform, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	SetActorTransform(NewTransform);
	ServerRPCPlayMontageOnRemotes_Implementation(Montage, StartSectionName);
}

void ACPCharacterBase::MulticastRPCPlayMontage_Implementation(UAnimMontage* Montage, FName StartSectionName)
{
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	PlayOrJumpMontage(Montage, StartSectionName);
}

void ACPCharacterBase::MulticastRPCPlayMontageOnRemotes_Implementation(UAnimMontage* Montage, FName StartSectionName)
{
	if (!IsLocallyControlled())
	{
		CP_NETLOG(LogCPNET, Log, TEXT("Actual Start"));
		PlayOrJumpMontage(Montage, StartSectionName);
	}
}

void ACPCharacterBase::PlayOrJumpMontage(UAnimMontage* Montage, FName StartSectionName)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (!AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->StopAllMontages(0.0f);
			AnimInstance->Montage_Play(Montage);
		}

		if (StartSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
		}
	}
}

bool ACPCharacterBase::IsOnSameTeamWith(APawn* OtherPawn) const
{
	if (GetPlayerState())
	{
		return OtherPawn->GetPlayerState() != nullptr;
	}
	else
	{
		return OtherPawn->GetPlayerState() == nullptr;
	}
}

void ACPCharacterBase::TriggerAttackEvent(float InChargeRate) const
{
	FGameplayEventData PayloadData;
	PayloadData.EventMagnitude = InChargeRate;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC != nullptr && IsValidChecked(ASC))
	{
		FScopedPredictionWindow NewScopedWindow(ASC, true);
		TArray<FGameplayAbilitySpec*> MeeleAttackAbilitySpecs;
		ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(GASTAG_CHARACTER_ACTION_PrimaryAttack), MeeleAttackAbilitySpecs);

		for (FGameplayAbilitySpec* AbilitySpecPtr : MeeleAttackAbilitySpecs)
		{
			if (AbilitySpecPtr->IsActive())
			{
				ASC->AbilitySpecInputPressed(*AbilitySpecPtr);
			}
			else
			{
				ASC->HandleGameplayEvent(GASTAG_CHARACTER_ACTION_PrimaryAttack, &PayloadData);
			}
		}
	}
}

UWeapon* ACPCharacterBase::GetCurrentWeapon()
{
	return WeaponComp->GetCurrentWeapon();
}

void ACPCharacterBase::SetWeaponMesh(UStaticMesh* NewWeaponMesh, const FTransform& NewMeshRelativeTransform)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	WeaponMesh->SetStaticMesh(NewWeaponMesh);
	WeaponMesh->SetRelativeTransform(NewMeshRelativeTransform);
}

float ACPCharacterBase::TakeDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AController* InDamageInstigatorController, AActor* InDamageCauser, TSubclassOf<UDamageType> InDamageTypeClass)
{
	Super::TakeDamage(InDamageAmount, FDamageEvent(InDamageTypeClass), InDamageInstigatorController, InDamageCauser);

	if (HasAuthority())
	{
		// Apply AttackDamage gameplay effect
		IAbilitySystemInterface* SourceGASInterface = Cast<IAbilitySystemInterface>(InDamageInstigatorController->GetPawn());
		if (SourceGASInterface)
		{
			UAbilitySystemComponent* SourceASC = SourceGASInterface->GetAbilitySystemComponent();
			UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent();
			if (SourceASC && TargetASC)
			{
				FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
				EffectContextHandle.AddInstigator(InDamageInstigatorController->GetPawn(), InDamageCauser);
				EffectContextHandle.AddSourceObject(this);
				EffectContextHandle.AddHitResult(InHitResult);
				
				FGameplayEffectSpecHandle DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(AttackDamageEffect, 1.0f, EffectContextHandle);
				if (DamageEffectSpecHandle.IsValid())
				{
					DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_CHARACTER_STAT_StunGauge, InStunValue);
					DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_CHARACTER_STAT_DamageToApply, InDamageAmount);
					SourceASC->BP_ApplyGameplayEffectSpecToTarget(DamageEffectSpecHandle, TargetASC);					
					ActivateDamageReactionAbility(EffectContextHandle, InDamageReactionAbilityTags);
				}
			}
		}
	}
	return InDamageAmount;
}

float ACPCharacterBase::TakeDamage(float InDamageAmount, FDamageEvent const& InDamageEvent, AController* InEventInstigatorController, AActor* InDamageCauser)
{
	Super::TakeDamage(InDamageAmount, InDamageEvent, InEventInstigatorController, InDamageCauser);
	CP_NETLOG(LogCP, Error, TEXT("Original TakeDamage function must not be called."));
	return InDamageAmount;
}

void ACPCharacterBase::MulticastRPCSpawnDamageIndicator_Implementation(float InDamageAmount, AController* InDamageInstigatorController)
{
	if (IsPlayerControlled() && IsLocallyControlled())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = InDamageInstigatorController ? InDamageInstigatorController->GetPawn() : nullptr;
	ADamageIndicator* DamageIndicator = GetWorld()->SpawnActor<ADamageIndicator>(DamageIndicatorClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParameters);
	DamageIndicator->SetDamageAmount(InDamageAmount);
}
