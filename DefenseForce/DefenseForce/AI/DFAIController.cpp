// Copyright 2024, Survedog. All rights reserved.


#include "AI/DFAIController.h"
#include "AbilitySystemInterface.h"

AActor* ADFAIController::GetAttackerActor() const
{
	return GetPawn();
}

UAbilitySystemComponent* ADFAIController::GetAttackerActorASC() const
{
	IAbilitySystemInterface* PawnGASInterface = Cast<IAbilitySystemInterface>(GetPawn());
	if (PawnGASInterface)
	{
		return PawnGASInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}
