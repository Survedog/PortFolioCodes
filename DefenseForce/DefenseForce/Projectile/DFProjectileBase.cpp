// Copyright 2024, Survedog. All rights reserved.


#include "Projectile/DFProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADFProjectileBase::ADFProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

AActor* ADFProjectileBase::GetAttackerActor() const
{
	return GetInstigator();
}

UAbilitySystemComponent* ADFProjectileBase::GetAttackerActorASC() const
{
	IAbilitySystemInterface* InstigatorGASInterface = Cast<IAbilitySystemInterface>(GetInstigator());
	if (InstigatorGASInterface)
	{
		return InstigatorGASInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ADFProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (Owner)
	{
		if (UPrimitiveComponent* CollisionComp = GetComponentByClass<UPrimitiveComponent>())
		{
			CollisionComp->IgnoreActorWhenMoving(Owner, true);
		}
	}
}

float ADFProjectileBase::ApplyPointDamage(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo, TSubclassOf<UDamageType> DamageTypeClass)
{
	return UGameplayStatics::ApplyPointDamage(DamagedActor, BaseDamage, HitFromDirection, HitInfo, GetInstigatorController(), this, DamageTypeClass);
}

bool ADFProjectileBase::ApplyRadialDamage(const UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
	return UGameplayStatics::ApplyRadialDamage(WorldContextObject, BaseDamage, Origin, DamageRadius, DamageTypeClass, IgnoreActors, this, GetInstigatorController(), bDoFullDamage, DamagePreventionChannel);
}

