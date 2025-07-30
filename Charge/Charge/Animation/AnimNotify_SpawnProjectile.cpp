// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_SpawnProjectile.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Log, TEXT("Start"));
	
	ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(MeshComp->GetOwner());
	if (AttackInterface)
	{
		UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
		if (Weapon)
		{
			if (Weapon->GetProjectileSpawnTiming() == EProjectileSpawnTiming::OnAnimNotify)
			{
				Weapon->SpawnProjectileOnAll();
			}
			else
			{
				CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Warning, TEXT("Weapon's ProjectileSpawnTiming is not set to OnAnimNotify."));
			}
		}
	}
}
