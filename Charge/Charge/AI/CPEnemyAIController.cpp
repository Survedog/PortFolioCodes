// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CPEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "Character/CPEnemyCharacter.h"
#include "Charge.h"

ACPEnemyAIController::ACPEnemyAIController() : BehaviorTreeAsset(nullptr), BlackBoardAsset(nullptr)
{
}

void ACPEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CP_NETLOG(LogCP, Log, TEXT("Start"));
	IAbilitySystemInterface* GASInterface = CastChecked<IAbilitySystemInterface>(InPawn);
	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	ensure(ASC);

	const UCPCharacterBaseAttributeSet* CharacterAttributeSet = ASC->GetSetChecked<UCPCharacterBaseAttributeSet>();
	CharacterAttributeSet->OnHpZero.AddDynamic(this, &ACPEnemyAIController::StopAI);

	ACPEnemyCharacter* CPEnemyCharacter = CastChecked<ACPEnemyCharacter>(InPawn);
	if (CPEnemyCharacter)
	{
		BehaviorTreeAsset = CPEnemyCharacter->GetBehaviorTreeAsset();
		BlackBoardAsset = CPEnemyCharacter->GetBlackBoardAsset();
		RunAI();
	}
}

void ACPEnemyAIController::RunAI()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	if (BehaviorTreeAsset)
	{
		bool RunResult = RunBehaviorTree(BehaviorTreeAsset);
		ensure(RunResult);
	}
}

void ACPEnemyAIController::StopAI()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}
