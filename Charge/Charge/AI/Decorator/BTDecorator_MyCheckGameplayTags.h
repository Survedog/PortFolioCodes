// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_CheckGameplayTagsOnActor.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_MyCheckGameplayTags.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UBTDecorator_MyCheckGameplayTags : public UBTDecorator_CheckGameplayTagsOnActor
{
	GENERATED_BODY()
	
public:
	UBTDecorator_MyCheckGameplayTags();

protected:
	virtual void SetOwner(AActor* InActorOwner) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) override;
	
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);
	void OnActorGameplayTagChange(const FGameplayTag GameplayTag, int32 TagCount);

protected:
	/** when observer can try to request abort? */
	UPROPERTY(Category = FlowControl, EditAnywhere)
	TEnumAsByte<EBTBlackboardRestart::Type> NotifyObserver;	

	UPROPERTY()
	TWeakObjectPtr<AActor> ActorOwner;

	UPROPERTY()
	TWeakObjectPtr<UBlackboardComponent> BlackboardComp;

	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandleMap;	
};
