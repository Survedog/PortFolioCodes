// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_TurnTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/KismetMathLibrary.h"
#include "Charge.h"

UBTTask_TurnTo::UBTTask_TurnTo()
{
	NodeName = TEXT("TurnTo");

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TurnTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TurnTo, BlackboardKey));
}

EBTNodeResult::Type UBTTask_TurnTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Do not call super function.
	const UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = BBComponent->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);
		if (TargetActor)
		{
			const FVector StartLocation = OwnerPawn->GetActorLocation();
			const FVector TargetLocation = TargetActor->GetActorLocation();
			OwnerPawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation));
		}
		else
		{
			CP_LOG(LogCPAI, Error, TEXT("Target is not assigned."));
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector StartLocation = OwnerPawn->GetActorLocation();
		const FVector TargetLocation = BBComponent->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		OwnerPawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation));
	}
	return EBTNodeResult::Succeeded;
}
