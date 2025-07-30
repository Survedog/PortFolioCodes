// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_MyCheckGameplayTags.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

UBTDecorator_MyCheckGameplayTags::UBTDecorator_MyCheckGameplayTags()
{
	bCreateNodeInstance = true;

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bAllowAbortChildNodes = true;
	bAllowAbortLowerPri = true;
	bAllowAbortNone = true;
	bNotifyActivation = true;
	bNotifyProcessed = true;
	FlowAbortMode = EBTFlowAbortMode::None;

	NodeName = TEXT("CheckGameplayTagsOnActor_WithNotifyObserver");
}

void UBTDecorator_MyCheckGameplayTags::SetOwner(AActor* InActorOwner)
{
	ActorOwner = InActorOwner;

	AAIController* AIController = Cast<AAIController>(ActorOwner.Get());
	if (AIController)
	{
		BlackboardComp = AIController->GetBlackboardComponent();
	}
}

void UBTDecorator_MyCheckGameplayTags::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (BlackboardComp.IsValid())
	{
		auto KeyID = ActorToCheck.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_MyCheckGameplayTags::OnBlackboardKeyValueChange));

		IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID()));
		if (!GASInterface)
		{
			return;
		}

		UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
		if (ASC)
		{
			for (const FGameplayTag& GameplayTag : GameplayTags)
			{
				const FDelegateHandle& DelegateHandle = ASC->RegisterGameplayTagEvent(GameplayTag).AddUObject(this, &UBTDecorator_MyCheckGameplayTags::OnActorGameplayTagChange);
				TagDelegateHandleMap.Add(GameplayTag, DelegateHandle);
			}
		}
	}
}

void UBTDecorator_MyCheckGameplayTags::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (BlackboardComp.IsValid())
	{
		BlackboardComp->UnregisterObserversFrom(this);

		IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID()));
		if (!GASInterface)
		{
			return;
		}

		UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
		if (ASC)
		{
			for (TPair<const FGameplayTag&, const FDelegateHandle&> TagDelegateHandlePair : TagDelegateHandleMap)
			{
				ASC->UnregisterGameplayTagEvent(TagDelegateHandlePair.Value, TagDelegateHandlePair.Key);
			}
		}
	}
}

void UBTDecorator_MyCheckGameplayTags::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	// Don't call super function.
	//if (BlackboardComp.IsValid())
	//{
	//	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID()));
	//	if (!GASInterface)
	//	{
	//		return;
	//	}

	//	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	//	if (ASC)
	//	{
	//		for (const FGameplayTag& GameplayTag : GameplayTags)
	//		{
	//			const FDelegateHandle& DelegateHandle = ASC->RegisterGameplayTagEvent(GameplayTag).AddUObject(this, &UBTDecorator_MyCheckGameplayTags::OnActorGameplayTagChange);
	//			TagDelegateHandleMap.Add(GameplayTag, DelegateHandle);
	//		}
	//	}
	//}
}

void UBTDecorator_MyCheckGameplayTags::OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult)
{
	// Don't call super function.
	//if (BlackboardComp.IsValid())
	//{
	//	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID()));
	//	if (!GASInterface)
	//	{
	//		return;
	//	}

	//	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	//	if (ASC)
	//	{
	//		for (TPair<const FGameplayTag&, const FDelegateHandle&> TagDelegateHandlePair : TagDelegateHandleMap)
	//		{
	//			ASC->UnregisterGameplayTagEvent(TagDelegateHandlePair.Value, TagDelegateHandlePair.Key);
	//		}
	//	}
	//}
}

EBlackboardNotificationResult UBTDecorator_MyCheckGameplayTags::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*)Blackboard.GetBrainComponent();
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (ActorToCheck.GetSelectedKeyID() == ChangedKeyID)
	{
		// can't simply use BehaviorComp->RequestExecution(this) here, we need to support condition/value change modes

		const EBTDecoratorAbortRequest RequestMode = (NotifyObserver == EBTBlackboardRestart::ValueChange) ? EBTDecoratorAbortRequest::ConditionPassing : EBTDecoratorAbortRequest::ConditionResultChanged;
		ConditionalFlowAbort(*BehaviorComp, RequestMode);
	}

	return EBlackboardNotificationResult::ContinueObserving;
}

void UBTDecorator_MyCheckGameplayTags::OnActorGameplayTagChange(const FGameplayTag GameplayTag, int32 TagCount)
{
	if (BlackboardComp.IsValid())
	{
		UObject* ActorToCheckKeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID());
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID(), ActorToCheckKeyValue);
	}
}
