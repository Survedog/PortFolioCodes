// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/InteractTriggerComponent.h"
#include "Character/CPPlayerCharacter.h"
#include "Interface/CPInteractiveObjectInterface.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Physics/CPCollision.h"
#include "Charge.h"

UInteractTriggerComponent::UInteractTriggerComponent()
{
	bWantsInitializeComponent = true;
	ShapeColor = FColor::Blue;
	SetCollisionProfileName(CPROFILE_TRIGGER);
}

void UInteractTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractTriggerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OnComponentBeginOverlap.AddDynamic(this, &UInteractTriggerComponent::OnComponentBeginOverlapCallback);
	OnComponentEndOverlap.AddDynamic(this, &UInteractTriggerComponent::OnComponentEndOverlapCallback);
}

void UInteractTriggerComponent::OnComponentBeginOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CP_LOG(LogCP, Log, TEXT("OverlapActor: %s / Component: %s"), *OtherActor->GetActorLabel(), *OtherComp->GetName());
	ACPPlayerCharacter* PlayerCharacter = Cast<ACPPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		ICPInteractiveObjectInterface* InteractInterface = Cast<ICPInteractiveObjectInterface>(GetOwner());
		if (InteractInterface)
		{
			PlayerCharacter->SetInteractTarget(InteractInterface);
		}
	}
}

void UInteractTriggerComponent::OnComponentEndOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CP_LOG(LogCP, Log, TEXT("OverlapActor: %s / Component: %s"), *OtherActor->GetActorLabel(), *OtherComp->GetName());
	ACPPlayerCharacter* PlayerCharacter = Cast<ACPPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		ICPInteractiveObjectInterface* InteractInterface = Cast<ICPInteractiveObjectInterface>(GetOwner());
		if (InteractInterface)
		{
			PlayerCharacter->ClearInteractTarget(InteractInterface);
		}
	}
}
