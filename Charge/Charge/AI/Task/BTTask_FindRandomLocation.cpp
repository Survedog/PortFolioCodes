// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_FindRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("FindRandomLocation");

	Origin.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, Origin), AActor::StaticClass());
	Origin.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, Origin));

	OutRandomLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, OutRandomLocation));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Do not call super function.
	UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();

	FVector OriginLocation = FVector::Zero();
	if (Origin.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{		
		if (AActor* OriginActor = Cast<AActor>(BBComponent->GetValue<UBlackboardKeyType_Object>(Origin.GetSelectedKeyID())))
		{
			OriginLocation = OriginActor->GetActorLocation();
		}
	}
	else if (Origin.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		OriginLocation = BBComponent->GetValue<UBlackboardKeyType_Vector>(Origin.GetSelectedKeyID());
	}
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(OwnerComp.GetWorld());
	if (NavSystem)
	{
		FNavLocation ResultLocation;
		bool bFoundLocation = NavSystem->GetRandomPointInNavigableRadius(OriginLocation, Radius, ResultLocation);
		if (bFoundLocation)
		{
			BBComponent->SetValue<UBlackboardKeyType_Vector>(OutRandomLocation.GetSelectedKeyID(), ResultLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	BBComponent->SetValue<UBlackboardKeyType_Vector>(OutRandomLocation.GetSelectedKeyID(), OriginLocation);
	return EBTNodeResult::Failed;
}

void UBTTask_FindRandomLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Origin.ResolveSelectedKey(*BBAsset);
		OutRandomLocation.ResolveSelectedKey(*BBAsset);
	}
}
