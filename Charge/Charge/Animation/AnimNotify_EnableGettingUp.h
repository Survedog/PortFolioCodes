// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnableGettingUp.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "EnableGettingUp"))
class CHARGE_API UAnimNotify_EnableGettingUp : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_EnableGettingUp();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY()
	TSubclassOf<class UGameplayAbility> KnockDownAbilityClass;
};
