// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_AttackHitCheck.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "Weapon/MeeleWeapon.h"
#include "Charge.h"

UAnimNotifyState_AttackHitCheck::UAnimNotifyState_AttackHitCheck()
{
	TriggerIndex = 0;
}

void UAnimNotifyState_AttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}

	CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Log, TEXT("Start"));
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(MeshComp->GetOwner());
		if (AttackInterface)
		{
			UMeeleWeapon* MeeleWeapon = Cast<UMeeleWeapon>(AttackInterface->GetCurrentWeapon());
			if (MeeleWeapon)
			{
				MeeleWeapon->ResetLastAttackTriggerLocation();
			}
		}
	}
}

void UAnimNotifyState_AttackHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp->GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(MeshComp->GetOwner());
		if (AttackInterface)
		{
			UMeeleWeapon* MeeleWeapon = Cast<UMeeleWeapon>(AttackInterface->GetCurrentWeapon());
			if (MeeleWeapon)
			{
				MeeleWeapon->MeeleAttackHitCheck(TriggerIndex);
			}
		}
	}
}
