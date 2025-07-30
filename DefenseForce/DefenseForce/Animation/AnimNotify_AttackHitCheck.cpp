// Copyright 2024, Survedog. All rights reserved.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interface/DFCharacterAnimAttackInterface.h"
#include "DFLog.h"

UAnimNotify_AttackHitCheck::UAnimNotify_AttackHitCheck()
{
	bShouldFireInEditor = false;
}

FString UAnimNotify_AttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheck");
}

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor->HasAuthority() && OwnerActor->Implements<UDFCharacterAnimAttackInterface>())
	{
		DF_LOG(LogDF, Log, TEXT("Check Attack Hit"));
		IDFCharacterAnimAttackInterface::Execute_AttackHitCheck(MeshComp->GetOwner());
	}
}
