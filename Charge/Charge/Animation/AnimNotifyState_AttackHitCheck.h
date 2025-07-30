// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Attack Hit Check State"))
class CHARGE_API UAnimNotifyState_AttackHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_AttackHitCheck();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	uint8 TriggerIndex;
};
