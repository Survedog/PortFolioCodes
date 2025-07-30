// Copyright 2024, Survedog. All rights reserved.


#include "Subsystem/TargetingInstanceSubsystem.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "DFLog.h"

AGameplayAbilityTargetActor* UTargetingInstanceSubsystem::GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass)
{
	AGameplayAbilityTargetActor* TargetActor = static_cast<const UTargetingInstanceSubsystem*>(this)->GetTargetActorFromClass(InTargetActorClass);
	if (!TargetActor)
	{
		TargetActor = GetWorld()->SpawnActor<AGameplayAbilityTargetActor>(InTargetActorClass);
		TAClassInstanceMap.Add(InTargetActorClass, TargetActor);
		DF_NETCUSTOMLOG(TargetActor, LogDF, Log, TEXT("Spawned Target Actor of class %s."), *InTargetActorClass->GetFName().ToString());
	}
	return TargetActor;
}

AGameplayAbilityTargetActor* UTargetingInstanceSubsystem::GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass) const
{
	const TObjectPtr<AGameplayAbilityTargetActor>* TargetActorPtr = TAClassInstanceMap.Find(InTargetActorClass);
	return TargetActorPtr ? *TargetActorPtr : nullptr;
}

void UTargetingInstanceSubsystem::Deinitialize()
{
	for (auto TAPair : TAClassInstanceMap)
	{
		if (AGameplayAbilityTargetActor* TargetActor = TAPair.Value)
		{
			TargetActor->Destroy();
		}
	}
}
