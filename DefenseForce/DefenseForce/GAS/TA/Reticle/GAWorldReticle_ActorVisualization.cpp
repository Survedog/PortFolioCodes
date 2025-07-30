// Copyright 2024, Survedog. All rights reserved.


#include "GAS/TA/Reticle/GAWorldReticle_ActorVisualization.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"

AGAWorldReticle_ActorVisualization::AGAWorldReticle_ActorVisualization(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), CurrentPlacedActorClass(nullptr), CurrentVisualizationMaterial(nullptr)
{
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule0"));
	CollisionComponent->InitCapsuleSize(0.f, 0.f);
	CollisionComponent->AlwaysLoadOnClient = true;
	CollisionComponent->SetUsingAbsoluteScale(true);
	CollisionComponent->AlwaysLoadOnServer = true;
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = CollisionComponent;
}

void AGAWorldReticle_ActorVisualization::InitializeVisualizationInformation(TSubclassOf<AActor> InPlacedActorClass, UMaterialInterface* InVisualizationMaterial)
{
	if (CurrentPlacedActorClass != InPlacedActorClass)
	{
		if (!MeshComps.IsEmpty())
		{
			DestroyMeshComponents();
		}

		if (InPlacedActorClass)
		{
			AttachMeshComponents(InPlacedActorClass, InVisualizationMaterial);
		}
	}
	else if (InVisualizationMaterial && CurrentVisualizationMaterial != InVisualizationMaterial)
	{
		for (UMeshComponent* MeshComp : MeshComps)
		{
			if (MeshComp)
			{
				int32 MaterialCount = MeshComp->GetMaterials().Num();
				for (int32 Idx = 0; Idx < MaterialCount; ++Idx)
				{
					MeshComp->SetMaterial(Idx, InVisualizationMaterial);
				}
			}
		}
	}
	CurrentPlacedActorClass = InPlacedActorClass;
	CurrentVisualizationMaterial = InVisualizationMaterial;
}

void AGAWorldReticle_ActorVisualization::AttachMeshComponents(TSubclassOf<AActor> InPlacedActorClass, UMaterialInterface* InVisualizationMaterial)
{
	check(InPlacedActorClass);
	if (AActor* TempVisualizationActor = GetWorld()->SpawnActor<AActor>(InPlacedActorClass))
	{
		//Get components
		TInlineComponentArray<UMeshComponent*> TempActorMeshComps;
		USceneComponent* MyRoot = GetRootComponent();
		TempVisualizationActor->GetComponents(TempActorMeshComps);
		check(MyRoot);

		AddTickPrerequisiteActor(TargetingActor);		//We want the reticle to tick after the targeting actor so that designers have the final say on the position
		TempVisualizationActor->SetRootComponent(NULL); //If we don't clear the root component explicitly, the component will be destroyed along with the original visualization actor.

		for (UMeshComponent* MeshComp : TempActorMeshComps)
		{
			//Disable collision on visualization mesh parts so it doesn't interfere with aiming or any other client-side collision/prediction/physics stuff
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);		//All mesh components are primitive components, so no cast is needed

			//Move components from one actor to the other, attaching as needed. Hierarchy should not be important, but we can do fixups if it becomes important later.
			MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			MeshComp->AttachToComponent(MyRoot, FAttachmentTransformRules::KeepRelativeTransform);
			MeshComp->Rename(nullptr, this);
			
			if (InVisualizationMaterial)
			{
				int32 MaterialCount = MeshComp->GetMaterials().Num();
				for (int32 Idx = 0; Idx < MaterialCount; ++Idx)
				{
					MeshComp->SetMaterial(Idx, InVisualizationMaterial);
				}
			}
			MeshComps.Add(MeshComp);
		}

		TempVisualizationActor->Destroy();
	}
}

void AGAWorldReticle_ActorVisualization::DestroyMeshComponents()
{
	for (UMeshComponent* MeshComp : MeshComps)
	{
		if (MeshComp)
		{
			MeshComp->DestroyComponent();
		}
	}
	MeshComps.Empty();
}
