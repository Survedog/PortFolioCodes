// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_AttackHitCheck();

protected:
	virtual FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
