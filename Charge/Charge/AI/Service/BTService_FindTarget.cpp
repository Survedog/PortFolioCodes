// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindTarget.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Physics/CPCollision.h"
#include "Interface/CPAIEnemyInterface.h"
#include "Engine/OverlapResult.h"
#include "Charge.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("FindTarget");
	bNotifyTick = true;
	Interval = 1.0f;
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	const float DetectRadius = 1500.0f;

	TArray<FOverlapResult> OutOverlapResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(OwnerPawn);
	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OutOverlapResults,
		OwnerPawn->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		//CCHANNEL_OVERLAPPAWN,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParams
	);

	TArray<APawn*> TargetCandidates;
	if (bHasOverlap)
	{
		for (const FOverlapResult& OverlapResult : OutOverlapResults)
		{
			APawn* OverlappedPawn = Cast<APawn>(OverlapResult.GetActor());
			if (OverlappedPawn && OverlappedPawn->IsPlayerControlled())
			{
				TargetCandidates.Add(OverlappedPawn);
			}
		}
	}

	APawn* FinalTarget = nullptr;
	float MinTargetDistance = BIG_NUMBER;
	for (APawn* TargetCandidate : TargetCandidates)
	{
		float TargetDistance = TargetCandidate->GetSquaredDistanceTo(OwnerComp.GetOwner());
		if (TargetDistance < MinTargetDistance)
		{
			FinalTarget = TargetCandidate;
			MinTargetDistance = TargetDistance;
		}
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), FinalTarget);
	ICPAIEnemyInterface* AIEnemyInterface = CastChecked<ICPAIEnemyInterface>(OwnerPawn);
	AIEnemyInterface->SetAttackTarget(FinalTarget);

#if ENABLE_DRAW_DEBUG
	//const FColor DrawColor = FinalTarget ? FColor::Green : FColor::Red;
	//DrawDebugSphere(GetWorld(), OwnerPawn->GetActorLocation(), DetectRadius, 16, DrawColor, false, 0.2f);
#endif
}