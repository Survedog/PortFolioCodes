// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "TargetingInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UTargetingInstanceSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	class AGameplayAbilityTargetActor* GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass);
	class AGameplayAbilityTargetActor* GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass) const;
	
	template<class T>
	T* GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass) { return Cast<T>(GetTargetActorFromClass(InTargetActorClass)); }

	template<class T>
	T* GetTargetActorFromClass(TSubclassOf<class AGameplayAbilityTargetActor> InTargetActorClass) const { return Cast<T>(GetTargetActorFromClass(InTargetActorClass)); }

protected:
	virtual void Deinitialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TMap<TSubclassOf<class AGameplayAbilityTargetActor>, TObjectPtr<class AGameplayAbilityTargetActor>> TAClassInstanceMap;
};
