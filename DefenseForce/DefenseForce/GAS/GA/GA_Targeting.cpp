// Copyright 2024, Survedog. All rights reserved.


#include "GAS/GA/GA_Targeting.h"
#include "GAS/AT/DFAT_WaitTargetData_ReusableTA.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"
#include "Interface/PlayerTowerControlInterface.h"
#include "Structure/DFTowerBase.h"
#include "GAS/DFGameplayTags.h"
#include "DefenseForce.h"
#include "DFLog.h"

void UGA_Targeting::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));

	if (!IsLocallyControlled())
	{
		return;
	}

	IPlayerTowerControlInterface* TowerControlInterface = CastChecked<IPlayerTowerControlInterface>(ActorInfo->AvatarActor);
	ControlledTower = Cast<ADFTowerBase>(TowerControlInterface->GetCurrentStructureUnderCursor());
	if (!ControlledTower)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	DFTargetActor = ControlledTower->GetAttackTargetActor();
	if (DFTargetActor)
	{
		GetAbilitySystemComponentFromActorInfo()->SpawnedTargetActors.Push(DFTargetActor);

		// TODO: Reuse AbilityTask
		UDFAT_WaitTargetData_ReusableTA* WaitTargetDataTask = UDFAT_WaitTargetData_ReusableTA::WaitTargetDataUsingReusableTA(this, FName("WaitAttackTargetData"), EGameplayTargetingConfirmation::Type::CustomMulti, DFTargetActor);
		if (WaitTargetDataTask)
		{
			WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_Targeting::OnTargetDataReadyCallback);
			WaitTargetDataTask->ReadyForActivation();
		}
	}
	else
	{
		DF_NETGASLOG(LogDFGAS, Error, TEXT("Target actor is nullptr."));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGA_Targeting::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start. EndState: %s"), bWasCancelled ? TEXT("Cancelled") : TEXT("Confirmed"));

	if (ControlledTower)
	{
		UAbilitySystemComponent* TowerASC = ControlledTower->GetAbilitySystemComponent();
		FGameplayTagContainer AttackTagContainer(GASTAG_Structure_Action_Attack);
		TowerASC->CancelAbilities(&AttackTagContainer);
	}

	UAbilitySystemComponent* PlayerASC = GetAbilitySystemComponentFromActorInfo_Checked();
	if (IsLocallyControlled() && DFTargetActor)
	{
		PlayerASC->SpawnedTargetActors.Remove(DFTargetActor);
		DFTargetActor->SetOwner(nullptr);
		DFTargetActor = nullptr;
		ControlledTower = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Targeting::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	if (ControlledTower && ControlledTower->ShouldConfirmTargetOnInputPressed())
	{
		DFTargetActor->ConfirmTargetingAndContinue();
	}
}

void UGA_Targeting::OnTargetDataReadyCallback_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	DF_NETGASLOG(LogDFGAS, Log, TEXT("Start"));
	FGameplayEventData Payload;
	Payload.TargetData = TargetDataHandle;
	UAbilitySystemComponent* TowerASC = ControlledTower->GetAbilitySystemComponent();
	if (!TowerASC->HasMatchingGameplayTag(GASTAG_Structure_Action_Attack))
	{
		TowerASC->HandleGameplayEvent(GASTAG_Structure_Action_Attack, &Payload);
	}
}
