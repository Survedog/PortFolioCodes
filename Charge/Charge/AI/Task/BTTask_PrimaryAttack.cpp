// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PrimaryAttack.h"
#include "AIController.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Charge.h"

UBTTask_PrimaryAttack::UBTTask_PrimaryAttack()
{
	NodeName = TEXT("PrimaryAttack");
	static ConstructorHelpers::FClassFinder<UGameplayAbility> PrimaryAttackAbilityClassRef(TEXT("/Game/Charge/Blueprint/GA/BPGA_PrimaryAttack.BPGA_PrimaryAttack_C"));
	if (PrimaryAttackAbilityClassRef.Class)
	{
		PrimaryAttackAbilityClass = PrimaryAttackAbilityClassRef.Class;
	}
}

EBTNodeResult::Type UBTTask_PrimaryAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Do not call super function.
	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!GASInterface)
	{
		CP_LOG(LogCPAI, Error, TEXT("Owner pawn doesn't implement IAbilitySystemInterface."));
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		CP_LOG(LogCPAI, Error, TEXT("Owner pawn doesn't have AbilitySystemComponent."));
		return EBTNodeResult::Failed;
	}

	FGameplayAbilitySpec* AttackAbilitySpec = ASC->FindAbilitySpecFromClass(PrimaryAttackAbilityClass);
	if (!AttackAbilitySpec)
	{
		CP_LOG(LogCPAI, Error, TEXT("Couldn't find primary attack ability spec from ASC."));
		return EBTNodeResult::Failed;
	}

	UGameplayAbility* PrimaryAttackAbility = AttackAbilitySpec->GetPrimaryInstance();
	PrimaryAttackAbility->OnGameplayAbilityEnded.AddLambda(
		[&](UGameplayAbility* Ability)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);

	if (AttackAbilitySpec->IsActive())
	{
		ASC->AbilitySpecInputPressed(*AttackAbilitySpec);
	}
	else
	{
		bool bActivated = ASC->TryActivateAbilityByClass(PrimaryAttackAbilityClass);
		if (!bActivated)
		{
			return EBTNodeResult::Failed;
		}
	}

	return EBTNodeResult::InProgress;
}
