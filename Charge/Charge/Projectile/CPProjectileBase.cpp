// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/CPProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Physics/CPCollision.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CPWeaponAttributeSet.h"
#include "GASTag/CPGameplayTag.h"
#include "Net/UnrealNetwork.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

ACPProjectileBase::ACPProjectileBase() :
	ChargeRate(0.0f),
	DamageAmount(0.0f),
	StunValue(0.0f),
	bShootOnBeginPlay(true),
	bDestroyOnHit(true),
	bEnableDestroyTimer(true),
	DestroyTime(5.0f),
	MaxDamageMultiplier(10.0f)
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCollisionProfileName(CPROFILE_PROJECTILE);
	Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACPProjectileBase::OnProjectileBeginOverlap);
	RootComponent = Capsule;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Capsule);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3000;
	ProjectileMovement->ProjectileGravityScale = 0.0f;	
}

void ACPProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CP_NETLOG(LogCP, Log, TEXT("Start"));

	if (bEnableDestroyTimer)
	{
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ACPProjectileBase::K2_DestroyActor, DestroyTime, false);
	}

	IAbilitySystemInterface* InstigatorGASInterface = Cast<IAbilitySystemInterface>(GetInstigator());
	if (InstigatorGASInterface)
	{
		UAbilitySystemComponent* ASC = InstigatorGASInterface->GetAbilitySystemComponent();
		if (ASC)
		{
			DamageAmount = ASC->GetSetChecked<UCPWeaponAttributeSet>()->GetDamage();
			StunValue = ASC->GetSetChecked<UCPWeaponAttributeSet>()->GetStunValue();
		}
	}
}

void ACPProjectileBase::SetChargeRate(const float InChargeRate)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	ChargeRate = InChargeRate;
	OnChargeAmountSet();
}

void ACPProjectileBase::BeginPlay()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::BeginPlay();
	
	if (bShootOnBeginPlay)
	{
		ShootProjectileToTarget();
	}
}

void ACPProjectileBase::ShootProjectileToTarget_Implementation()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	FVector ToTarget = ProjectileTargetLocation - GetActorLocation();
	SetActorRotation(ToTarget.Rotation());
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->GetMaxSpeed();
	//DrawDebugLine(GetWorld(), GetActorLocation(), ProjectileTargetLocation, FColor::Blue, false, 1.0f);
}

void ACPProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor != GetInstigator())
	{
		if (HasAuthority())
		{
			IAbilitySystemInterface* TargetGASInterface = Cast<IAbilitySystemInterface>(OtherActor);
			UAbilitySystemComponent* TargetASC = TargetGASInterface ? TargetGASInterface->GetAbilitySystemComponent() : nullptr;
			if (TargetASC)
			{
				if (TargetASC->HasMatchingGameplayTag(GASTAG_CHARACTER_STATE_Invincible_Evasive))
				{
					return;
				}

				CP_NETLOG(LogCP, Log, TEXT("HitTarget: %s"), *OtherActor->GetName());
				ICPCharacterAttackInterface* AttackInterface = CastChecked<ICPCharacterAttackInterface>(GetInstigator());
				AttackInterface->GetCurrentWeapon()->DealDamage(GetDamageAmount(), GetStunValue(), SweepResult, GetDamageReactionAbilityTag(), this, GetDamageType());
			}
			OnProjectileHit.Broadcast(SweepResult);
		}

		if (bDestroyOnHit)
		{
			SetActorHiddenInGame(true);
			Destroy();
		}
	}
}

float ACPProjectileBase::GetDamageAmount_Implementation()
{
	return DamageAmount;
}

float ACPProjectileBase::GetStunValue_Implementation()
{
	return StunValue;
}