// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/MeeleWeapon.h"
#include "Character/CPCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Attribute/CPWeaponAttributeSet.h"
#include "Physics/CPCollision.h"
#include "CollisionDebugDrawingPublic.h"
#include "Charge.h"

UMeeleWeapon::UMeeleWeapon()
{
	ProjectileSpawnTiming = EProjectileSpawnTiming::None;
	AttackEndTiming = EAttackEndTiming::OnMontageEnded;
}

void UMeeleWeapon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == TEXT("MeeleAttackHitTriggers"))
	{
		if (CPCharacterOwner.IsValid())
		{
			for (const FMeeleAttackHitTriggerInfo& HitTriggerInfo : MeeleAttackHitTriggers)
			{
				const FTransform& SocketTransform = CPCharacterOwner->GetMesh()->GetSocketTransform(HitTriggerInfo.SocketName);
				const FTransform& TriggerTransform = UKismetMathLibrary::ComposeTransforms(HitTriggerInfo.RelativeTransform, SocketTransform);
				DrawDebugCapsule(GetWorld(), TriggerTransform.GetLocation(), HitTriggerInfo.CapsuleHalfHeight, HitTriggerInfo.CapsuleRadius, TriggerTransform.GetRotation(), FColor::MakeRandomColor(), false, 5.0f);
			}
		}
	}
}

void UMeeleWeapon::OnWeaponObtained_Implementation(UCharacterWeaponComponent* NewWeaponComp)
{
	Super::OnWeaponObtained_Implementation(NewWeaponComp);
	
	LastAttackTriggerLocation.SetNumUninitialized(MeeleAttackHitTriggers.Num());
}

void UMeeleWeapon::OnWeaponDropped_Implementation(UCharacterWeaponComponent* OldWeaponComp)
{
	Super::OnWeaponDropped_Implementation(OldWeaponComp);

	LastAttackTriggerLocation.Reset();
}

void UMeeleWeapon::MeeleAttackHitCheck(const uint8 TriggerIndex)
{
	if (!CPCharacterOwner->HasAuthority())
	{
		return;
	}

	TArray<FHitResult> OutHitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(CPCharacterOwner.Get());
	CollisionQueryParams.AddIgnoredActors(MeeleAttackHitTargets.Array());

	const FMeeleAttackHitTriggerInfo* Trigger = MeeleAttackHitTriggers.IsValidIndex(TriggerIndex) ? &MeeleAttackHitTriggers[TriggerIndex] : nullptr;
	if (Trigger)
	{
		UAbilitySystemComponent* ASC = CPCharacterOwner->GetAbilitySystemComponent();
		if (ASC)
		{
			const UCPWeaponAttributeSet* WeaponAttributeSet = ASC->GetSetChecked<UCPWeaponAttributeSet>();
			const float AttackRangeMultiplier = WeaponAttributeSet->GetMeeleAttackRangeMultiplier();

			FTransform TriggerRelativeTransform = Trigger->RelativeTransform;
			TriggerRelativeTransform.AddToTranslation(TriggerRelativeTransform.GetRotation().GetUpVector() * (AttackRangeMultiplier - 1.0f) * Trigger->CapsuleHalfHeight);
			const FTransform& SocketTransform = CPCharacterOwner->GetMesh()->GetSocketTransform(Trigger->SocketName);
			const FTransform& TriggerWorldTransform = UKismetMathLibrary::ComposeTransforms(TriggerRelativeTransform, SocketTransform);

			const FVector SweepEnd = TriggerWorldTransform.GetLocation();
			const FVector SweepStart = LastAttackTriggerLocation.IsValidIndex(TriggerIndex) ? LastAttackTriggerLocation[TriggerIndex] : SweepEnd;
			LastAttackTriggerLocation[TriggerIndex] = SweepEnd;

			const float CapsuleHalfHeight = Trigger->CapsuleHalfHeight * AttackRangeMultiplier;
			const float CapsuleRadius = Trigger->CapsuleRadius;
			FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
			bool bIsHit = GetWorld()->SweepMultiByChannel(OutHitResults, SweepStart, SweepEnd, TriggerWorldTransform.GetRotation(), CCHANNEL_ATTACK, CapsuleShape, CollisionQueryParams);
			if (bIsHit)
			{
				const float DamageAmount = WeaponAttributeSet->GetDamage();
				const float StunValue = WeaponAttributeSet->GetStunValue();
				for (const FHitResult HitResult : OutHitResults)
				{
					if (HitResult.GetActor())
					{
						DealDamage(DamageAmount, StunValue, HitResult, GetDamageReactionAbilityTag(), CPCharacterOwner.Get(), GetMeeleAttackDamageType());
						MeeleAttackHitTargets.Add(HitResult.GetActor());
					}
				}
			}

#if ENABLE_DRAW_DEBUG
			DrawCapsuleSweeps(GetWorld(), SweepStart, SweepEnd, CapsuleHalfHeight, CapsuleRadius, TriggerWorldTransform.GetRotation(), OutHitResults, 1.0f);
#endif
		}
	}
}

void UMeeleWeapon::ResetLastAttackTriggerLocation()
{
	for (int32 Idx = 0; Idx < MeeleAttackHitTriggers.Num(); Idx++)
	{
		const FMeeleAttackHitTriggerInfo& AttackHitTriggerInfo = MeeleAttackHitTriggers[Idx];
		const FTransform& SocketTransform = CPCharacterOwner->GetMesh()->GetSocketTransform(AttackHitTriggerInfo.SocketName);
		LastAttackTriggerLocation[Idx] = UKismetMathLibrary::ComposeTransforms(AttackHitTriggerInfo.RelativeTransform, SocketTransform).GetLocation();
	}
}

TSubclassOf<UDamageType> UMeeleWeapon::GetMeeleAttackDamageType_Implementation() const
{
	return UDamageType::StaticClass();
}

void UMeeleWeapon::PrimaryAttack_Implementation(int32 InComboCount)
{
	MeeleAttackHitTargets.Reset();

	Super::PrimaryAttack_Implementation(InComboCount);
}

void UMeeleWeapon::EndPrimaryAttack_Implementation()
{
	Super::EndPrimaryAttack_Implementation();
	MeeleAttackHitTargets.Reset();
}