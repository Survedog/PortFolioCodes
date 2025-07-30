// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_WaitAttackInput.h"
#include "Interface/CPCharacterAttackInterface.h"
#include "Weapon/Weapon.h"
#include "Charge.h"

void UAnimNotifyState_WaitAttackInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Log, TEXT("Start"));
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(MeshComp->GetOwner());
	if (AttackInterface)
	{
		UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
		if (Weapon)
		{
			if (Weapon->HasAttackInput())
			{
				Weapon->ProceedComboAttack();
			}
			else
			{
				Weapon->SetIsWaitingForNextAttack(true);
			}
		}
	}
}

void UAnimNotifyState_WaitAttackInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Log, TEXT("Start"));
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ICPCharacterAttackInterface* AttackInterface = Cast<ICPCharacterAttackInterface>(MeshComp->GetOwner());
	if (AttackInterface)
	{
		UWeapon* Weapon = AttackInterface->GetCurrentWeapon();
		if (Weapon)
		{
			Weapon->SetIsWaitingForNextAttack(false);
		}
	}
}
