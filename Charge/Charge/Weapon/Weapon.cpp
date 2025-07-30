// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Character/CPCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Interface/CPFireProjectileInterface.h"
#include "Projectile/CPProjectileBase.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Attribute/CPWeaponAttributeSet.h"
#include "GASTag/CPGameplayTag.h"
#include "Game/CPDamageTypeSubsystem.h"
#include "Charge.h"

UWeapon::UWeapon() : 
	CPCharacterOwner(nullptr),
	OwnerWeaponComp(nullptr),
	PrimaryAttackMontage(nullptr),
	WeaponMesh(nullptr),
	MeshRelativeTransform(FTransform::Identity)
{	
	bIsProjectileReplicated = false;
	WeaponType = EWeaponType::None;
	CurrentAttackChargeRate = 0.0f;
	bIsAttacking = false;
	bIsWaitingForNextAttack = true;
	bHasAttackInput = false;
	bStopMovementOnAttack = false;
	ProjectileSpawnTiming = EProjectileSpawnTiming::OnAttackStarted;
	AttackEndTiming = EAttackEndTiming::OnMontageEnded;
	MaxPrimaryAttackComboCount = 1;
	CurrentComboCount = 0;

	BaseChargeSpeed = 0.0f;
	BaseMaxChargeAmount = 0.0f;
	BaseDamage = 0.0f;
	BaseStunValue = 0.0f;
	MaxAttackDamageMultiplier = 1.0f;
	MaxStunValueMultiplier = 1.0f;	

	static ConstructorHelpers::FClassFinder<UGameplayEffect> MultiplyDamageAmountEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_MultiplyDamageAmount.BPGE_MultiplyDamageAmount_C"));
	if (MultiplyDamageAmountEffectClassRef.Class)
	{
		MultiplyDamageAmountEffect = MultiplyDamageAmountEffectClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UGameplayEffect> MultiplyStunValueEffectClassRef(TEXT("/Game/Charge/Blueprint/GE/BPGE_MultiplyStunValue.BPGE_MultiplyStunValue_C"));
	if (MultiplyStunValueEffectClassRef.Class)
	{
		MultiplyStunValueEffect = MultiplyStunValueEffectClassRef.Class;
	}
}

AActor* UWeapon::GetActorOwner() const
{
	AActor* Owner = Cast<AActor>(GetOuter());
	if (!Owner)
	{
		Owner = CPCharacterOwner.Get();
	}
	return Owner;
}

bool UWeapon::CanDealDamageTo_Implementation(APawn* OtherPawn) const
{
	return CPCharacterOwner.IsValid() ? !CPCharacterOwner->IsOnSameTeamWith(OtherPawn) : false;
}

int32 UWeapon::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	AActor* Owner = GetActorOwner();
	return Owner ? Owner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

bool UWeapon::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	AActor* Owner = GetActorOwner();
	if (Owner)
	{
		UNetDriver* NetDriver = Owner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		}
	}
	return false;
}

void UWeapon::StartComboAttack_Implementation(float InChargeRate)
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		EndPrimaryAttack();
		return;
	}
	else if (CurrentComboCount != 0)
	{
		CP_LOG(LogCP, Error, TEXT("StartComboAttack can only be called at the start of combo attack."));
		EndPrimaryAttack();
		return;
	}
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));

	bIsAttacking = true;
	CurrentComboCount = 0;
	CurrentAttackChargeRate = InChargeRate;
	
	if (bStopMovementOnAttack)
	{
		CPCharacterOwner->SetIgnoreMovement(true);
	}

	if (CPCharacterOwner->HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = CPCharacterOwner->GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle DamageEffectSpecHandle = ASC->MakeOutgoingSpec(MultiplyDamageAmountEffect, 1.0f, FGameplayEffectContextHandle());
			DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_Damage, GetAttackDamageMultiplier());
			ActiveMultiplyDamageAmountEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(DamageEffectSpecHandle);

			FGameplayEffectSpecHandle StunValueEffectSpecHandle = ASC->MakeOutgoingSpec(MultiplyStunValueEffect, 1.0f, FGameplayEffectContextHandle());
			StunValueEffectSpecHandle.Data->SetSetByCallerMagnitude(GASTAG_WEAPON_STAT_StunValue, GetStunValueMultiplier());
			ActiveMultiplyStunValueEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(StunValueEffectSpecHandle);
		}
	}
	OnAttackStart.Broadcast();
	ProceedComboAttack();
}

void UWeapon::ProceedComboAttack()
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		EndPrimaryAttack();
		return;
	}
	else if (CurrentComboCount >= MaxPrimaryAttackComboCount)
	{
		CP_LOG(LogCP, Error, TEXT("CurrentComboCount exceeded max combo count."));
		EndPrimaryAttack();
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	bHasAttackInput = false;
	bIsWaitingForNextAttack = false;

	CurrentComboCount++;
	PrimaryAttack(CurrentComboCount);
	OnAttackComboProceed.Broadcast();
}

void UWeapon::EndPrimaryAttack_Implementation()
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	if (bIsAttacking)
	{
		if (bStopMovementOnAttack)
		{
			CPCharacterOwner->SetIgnoreMovement(false);
		}

		if (CPCharacterOwner->HasAuthority())
		{
			UAbilitySystemComponent* ASC = CPCharacterOwner->GetAbilitySystemComponent();
			if (ASC)
			{
				if (ActiveMultiplyDamageAmountEffectHandle.IsValid())
				{
					ensure(ASC->RemoveActiveGameplayEffect(ActiveMultiplyDamageAmountEffectHandle));
					ActiveMultiplyDamageAmountEffectHandle.Invalidate();
				}

				if (ActiveMultiplyStunValueEffectHandle.IsValid())
				{
					ensure(ASC->RemoveActiveGameplayEffect(ActiveMultiplyStunValueEffectHandle));
					ActiveMultiplyStunValueEffectHandle.Invalidate();
				}
			}
		}
		bIsAttacking = false;
		bIsWaitingForNextAttack = true;
		CurrentComboCount = 0;
		CurrentAttackChargeRate = 0.0f;
		OnAttackEnd.Broadcast();
	}
}

void UWeapon::SpawnProjectileOnAll()
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	TSubclassOf<ACPProjectileBase> PrimaryAttackProjectileClass = GetProjectileClass();
	ICPFireProjectileInterface* FireInterface = Cast<ICPFireProjectileInterface>(CPCharacterOwner.Get());
	if (PrimaryAttackProjectileClass && FireInterface)
	{
		if (bIsProjectileReplicated)
		{
			if (CPCharacterOwner->HasAuthority())
			{
				SpawnProjectile(PrimaryAttackProjectileClass, FireInterface->GetProjectileTargetLocation(), CurrentAttackChargeRate);
			}
		}
		else
		{
			if (ProjectileSpawnTiming == EProjectileSpawnTiming::OnAnimNotify)
			{
				SpawnProjectile(PrimaryAttackProjectileClass, FireInterface->GetProjectileTargetLocation(), CurrentAttackChargeRate);
			}
			else
			{
				if (CPCharacterOwner->HasAuthority())
				{
					MulticastRPCSpawnProjectileOnRemotes(PrimaryAttackProjectileClass, FireInterface->GetProjectileTargetLocation(), CurrentAttackChargeRate);
				}
				if (CPCharacterOwner->IsLocallyControlled())
				{
					SpawnProjectile(PrimaryAttackProjectileClass, FireInterface->GetProjectileTargetLocation(), CurrentAttackChargeRate);
				}
			}
		}

		if (AttackEndTiming == EAttackEndTiming::OnProjectileSpawned)
		{
			EndPrimaryAttack();
		}
	}
}

void UWeapon::DealDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AActor* InDamageCauser, TSubclassOf<UDamageType> InDamageTypeClass)
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return;
	}

	if (APawn* TargetPawn = Cast<APawn>(InHitResult.GetActor()))
	{
		if (!CanDealDamageTo(TargetPawn))
		{
			return;
		}
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	UAbilitySystemComponent* SourceASC = CPCharacterOwner->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Error, TEXT("Source character doens't have ASC instance."));
		return;
	}
	
	ACPCharacterBase* TargetCPCharacter = Cast<ACPCharacterBase>(InHitResult.GetActor());
	if (TargetCPCharacter)
	{

		TargetCPCharacter->TakeDamage(InDamageAmount, InStunValue, InHitResult, InDamageReactionAbilityTags, CPCharacterOwner->GetController(), InDamageCauser, InDamageTypeClass);
	}
}

TSubclassOf<ACPProjectileBase> UWeapon::GetProjectileClass_Implementation() const
{
	return nullptr;
}

void UWeapon::PrimaryAttack_Implementation(int32 InComboCount)
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start. InComboCount: %d"), InComboCount);		
	//bHasAttackInput = false;
	//bIsWaitingForNextAttack = false;
	if (UAnimMontage* AttackMontage = GetPrimaryAttackMontage())
	{
		PlayAttackMontageOnAll(AttackMontage, InComboCount);
	}

	if (ProjectileSpawnTiming == EProjectileSpawnTiming::OnAttackStarted)
	{
		SpawnProjectileOnAll();
	}
}

UAnimMontage* UWeapon::GetPrimaryAttackMontage_Implementation() const
{
	return PrimaryAttackMontage;
}

void UWeapon::OnAttackMontageBlendOutCallback(UAnimMontage* Montage, bool bInterrupted)
{
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	bIsWaitingForNextAttack = false;
	bHasAttackInput = false;
}

void UWeapon::OnAttackMontageEndedCallback(UAnimMontage* Montage, bool bInterrupted)
{
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	EndPrimaryAttack();
}

void UWeapon::PlayAttackMontageOnAll(UAnimMontage* InAttackMontage, int32 InComboCount)
{
	if (!CPCharacterOwner.IsValid())
	{
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	PlayAttackMontage(InAttackMontage, InComboCount);
	if (CPCharacterOwner->HasAuthority())
	{
		MulticastRPCPlayAttackMontageOnRemotes(InAttackMontage, InComboCount);
	}
}

void UWeapon::MulticastRPCPlayAttackMontageOnRemotes_Implementation(UAnimMontage* InAttackMontage, int32 InComboCount)
{
	if (CPCharacterOwner->IsLocallyControlled())
	{
		return;
	}
	PlayAttackMontage(InAttackMontage, InComboCount);
}

void UWeapon::PlayAttackMontage(UAnimMontage* InAttackMontage, int32 InComboCount)
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start. InComboCount: %d"), InComboCount);
	FName MontageSectionName(FString::Printf(TEXT("Combo%d"), InComboCount));
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("MontageSectionName: %s"), *MontageSectionName.ToString());
	CPCharacterOwner->PlayOrJumpMontage(InAttackMontage, MontageSectionName);

	if (AttackEndTiming == EAttackEndTiming::OnMontageEnded && CPCharacterOwner->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		CPCharacterOwner->GetAnimInstance()->Montage_GetBlendingOutDelegate(InAttackMontage)->BindUObject(this, &UWeapon::OnAttackMontageBlendOutCallback);
		CPCharacterOwner->GetAnimInstance()->Montage_GetEndedDelegate(InAttackMontage)->BindUObject(this, &UWeapon::OnAttackMontageEndedCallback);
	}
}

void UWeapon::MulticastRPCSpawnProjectileOnRemotes_Implementation(TSubclassOf<class ACPProjectileBase> InProjectileClass, FVector_NetQuantize InTargetLocation, float InChargeRate)
{
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	if (CPCharacterOwner->IsLocallyControlled())
	{
		return;
	}
	SpawnProjectile(InProjectileClass, InTargetLocation, InChargeRate);
}

ACPProjectileBase* UWeapon::SpawnProjectile_Implementation(TSubclassOf<class ACPProjectileBase> InProjectileClass, const FVector_NetQuantize InTargetLocation, float InChargeRate)
{
	if (!CPCharacterOwner.IsValid())
	{
		CP_LOG(LogCP, Error, TEXT("CPCharacterOwner is not valid."));
		return nullptr;
	}

	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start. InChargeRate: %f"), InChargeRate);
	APawn* Instigator = CPCharacterOwner.Get();
	AActor* Owner = Instigator ? Instigator->GetController() : nullptr;
	ICPFireProjectileInterface* FireInterface = Cast<ICPFireProjectileInterface>(Instigator);
	if (FireInterface)
	{
		ACPProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<ACPProjectileBase>(InProjectileClass, FTransform::Identity, Owner, Instigator);
		if (Projectile)
		{
			if (CPCharacterOwner->HasAuthority())
			{
				Projectile->SetReplicates(bIsProjectileReplicated);
			}
			Projectile->SetChargeRate(InChargeRate);
			Projectile->SetProjectileTargetLocation(InTargetLocation);			

			const FTransform SpawnTransform(FRotator::ZeroRotator, FireInterface->GetProjectileSpawnLocation());
			Projectile->FinishSpawning(SpawnTransform);
			return Projectile;
		}
	}
	return nullptr;
}

void UWeapon::OnWeaponObtained_Implementation(UCharacterWeaponComponent* NewWeaponComp)
{
	check(NewWeaponComp);
	OwnerWeaponComp = NewWeaponComp;
	CPCharacterOwner = NewWeaponComp->GetCPCharacterOwner();
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("End"));
}

void UWeapon::OnWeaponDropped_Implementation(UCharacterWeaponComponent* OldWeaponComp)
{
	CP_NETCUSTOMLOG(CPCharacterOwner, LogCP, Log, TEXT("Start"));
	if (OwnerWeaponComp.IsValid())
	{
		OnWeaponUnequipped();
	}
	OwnerWeaponComp = nullptr;
	CPCharacterOwner = nullptr;

	// TODO: Destory Object
	//MarkAsGarbage();
	//MarkPendingKill();
}

void UWeapon::OnWeaponEquipped_Implementation()
{
}

void UWeapon::OnWeaponUnequipped_Implementation()
{
}

float UWeapon::GetAttackDamageMultiplier_Implementation() const
{
	return FMath::Lerp<float, float>(1.0f, MaxAttackDamageMultiplier, CurrentAttackChargeRate);
}

float UWeapon::GetStunValueMultiplier_Implementation() const
{
	return FMath::Lerp<float, float>(1.0f, MaxStunValueMultiplier, CurrentAttackChargeRate);
}
