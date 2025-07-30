// Copyright 2024, Survedog. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ShootActorToTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEFENSEFORCE_API UAnimNotify_ShootActorToTarget : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_ShootActorToTarget();

protected:
	virtual FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
