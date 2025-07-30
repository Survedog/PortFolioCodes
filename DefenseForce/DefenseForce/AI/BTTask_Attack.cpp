// Copyright 2024, Survedog. All rights reserved.


#include "AI/BTTask_Attack.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GAS/DFGameplayTags.h"
#include "AIController.h"
#include "DFLog.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");

	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Do not call super function.
	AAIController* AIController = CastChecked<AAIController>(OwnerComp.GetOwner());
	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!GASInterface)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	TArray<FGameplayAbilitySpec*> AttackAbilitySpecs;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(GASTAG_Character_Action_Attack), AttackAbilitySpecs);
	for (FGameplayAbilitySpec* AbilitySpec : AttackAbilitySpecs)
	{
		if (UGameplayAbility* AttackAbility = AbilitySpec->GetPrimaryInstance())
		{
			ActivatedAbilities.Add(AttackAbility);
			AbilityEndDelegateHandle = AttackAbility->OnGameplayAbilityEndedWithData.AddLambda(
					[&](const FAbilityEndedData& AbilityEndedData)
					{
						//AbilityEndedData.AbilityThatEnded->OnGameplayAbilityEndedWithData.RemoveAll(this);
						DF_LOG(LogDF, Log, TEXT("Attack Ability Ended Callback (%s)"), AbilityEndedData.bWasCancelled ? TEXT("Failed") : TEXT("Succeeded"));
						FinishLatentTask(OwnerComp, AbilityEndedData.bWasCancelled ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
					}
				);

			bool bActivated = ASC->TryActivateAbility(AbilitySpec->Handle);
			if (!bActivated)
			{
				return EBTNodeResult::Failed;
			}
		}
	}
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Do not call super function.
	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetOwner());
	if (!GASInterface)
	{
		return EBTNodeResult::Aborted;
	}

	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Aborted;
	}

	for (UGameplayAbility* AttackAbility : ActivatedAbilities)
	{
		if (AttackAbility)
		{
			AttackAbility->OnGameplayAbilityEndedWithData.Remove(AbilityEndDelegateHandle);
			AbilityEndDelegateHandle = AttackAbility->OnGameplayAbilityEndedWithData.AddLambda(
					[&](const FAbilityEndedData& AbilityEndedData)
					{
						//AbilityEndedData.AbilityThatEnded->OnGameplayAbilityEndedWithData.RemoveAll(this);
						DF_LOG(LogDF, Log, TEXT("Attack Ability Ended Callback (Aborted)"));
						FinishLatentAbort(OwnerComp);
					}
				);
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// Do not call super function.
	AbilityEndDelegateHandle.Reset();
}
