// Copyright 2024, Survedog. All rights reserved.


#include "Animation/AnimNotify_ShootActorToTarget.h"
#include "Interface/DFProjectileTowerInterface.h"
#include "Projectile/DFProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

UAnimNotify_ShootActorToTarget::UAnimNotify_ShootActorToTarget()
{
	bShouldFireInEditor = false;
}

FString UAnimNotify_ShootActorToTarget::GetNotifyName_Implementation() const
{
	return TEXT("ShootActorToTarget");
}

void UAnimNotify_ShootActorToTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	IDFProjectileTowerInterface* ProjectileThrowerInterface = Cast<IDFProjectileTowerInterface>(MeshComp->GetOwner());
	if (ProjectileThrowerInterface)
	{
		const TSubclassOf<AActor> ProjectileClass = ProjectileThrowerInterface->GetProjectileActorClass();
		const FVector SpawnLocation = ProjectileThrowerInterface->GetProjectileSpawnLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = MeshComp->GetOwner();
		SpawnParams.Instigator = Cast<APawn>(MeshComp->GetOwner());
		
		ADFProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<ADFProjectileBase>(ProjectileClass, SpawnParams);
		if (SpawnedProjectile)
		{
			UProjectileMovementComponent* ProjectileMovementComp = SpawnedProjectile->GetProjectileMovement();
			check(ProjectileMovementComp);
			ProjectileMovementComp->Velocity = ProjectileThrowerInterface->GetProjectileLaunchVelocity();
		}
	}
}
