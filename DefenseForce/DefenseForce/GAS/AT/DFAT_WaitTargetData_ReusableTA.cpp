// Copyright 2024, Survedog. All rights reserved.


#include "GAS/AT/DFAT_WaitTargetData_ReusableTA.h"
#include "AbilitySystemComponent.h"
#include "GAS/TA/DFGATA_Trace.h"
#include "DFLog.h"

UDFAT_WaitTargetData_ReusableTA::UDFAT_WaitTargetData_ReusableTA() : TargetActor(nullptr)
{
	ConfirmationType = EGameplayTargetingConfirmation::Instant;
}

UDFAT_WaitTargetData_ReusableTA* UDFAT_WaitTargetData_ReusableTA::WaitTargetDataUsingReusableTA(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor)
{
	check(InTargetActor);
	check(OwningAbility);
	DF_NETCUSTOMLOG(OwningAbility->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	UDFAT_WaitTargetData_ReusableTA* MyObj = NewAbilityTask<UDFAT_WaitTargetData_ReusableTA>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TargetActor = InTargetActor;
	MyObj->ConfirmationType = ConfirmationType;

	if (InTargetActor->GenericDelegateBoundASC != MyObj->AbilitySystemComponent.Get())
	{
		InTargetActor->GenericDelegateBoundASC = MyObj->AbilitySystemComponent.Get();
	}
	return MyObj;
}

void UDFAT_WaitTargetData_ReusableTA::Activate()
{
	check(Ability);
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	if (!IsValid(this))
	{
		return;
	} 

	if (TargetActor)
	{
		InitializeTargetActor(TargetActor);
		RegisterTargetDataCallbacks();
		FinalizeTargetActor(TargetActor); // Note that the call to FinalizeTargetActor, this task could finish and our owning ability may be ended.
	}
	else
	{
		EndTask();
	}
}

/** Valid TargetData was replicated to use (we are server, was sent from client) */
void UDFAT_WaitTargetData_ReusableTA::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	check(AbilitySystemComponent.IsValid());
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	FGameplayAbilityTargetDataHandle MutableData = Data;

	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	/**
	 *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

/** Client canceled this Targeting Task (we are the server) */
void UDFAT_WaitTargetData_ReusableTA::OnTargetDataReplicatedCancelledCallback()
{
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

/** The TargetActor has called back with valid target data */
void UDFAT_WaitTargetData_ReusableTA::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.IsValid());
	check(Ability);
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent.Get(), ShouldReplicateDataToServer());

	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		check(TargetActor);
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			FGameplayTag ApplicationTag; // Fixme: where would this be useful?
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UDFAT_WaitTargetData_ReusableTA::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.IsValid());
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo()->GetOwner(), LogDFGAS, Log, TEXT("Start"));

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	if (IsPredictingClient())
	{
		check(TargetActor);
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
}

void UDFAT_WaitTargetData_ReusableTA::ExternalConfirm(bool bEndTask)
{
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}
	Super::ExternalConfirm(bEndTask);
}

void UDFAT_WaitTargetData_ReusableTA::ExternalCancel()
{
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	Super::ExternalCancel();
}

void UDFAT_WaitTargetData_ReusableTA::InitializeTargetActor(AGameplayAbilityTargetActor* InTargetActor) const
{
	check(InTargetActor);
	check(Ability);
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	InTargetActor->PrimaryPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	if (!InTargetActor->TargetDataReadyDelegate.IsBoundToObject(this))
	{
		InTargetActor->TargetDataReadyDelegate.AddUObject(const_cast<UDFAT_WaitTargetData_ReusableTA*>(this), &UDFAT_WaitTargetData_ReusableTA::OnTargetDataReadyCallback);
	}

	if (!InTargetActor->CanceledDelegate.IsBoundToObject(this))
	{
		InTargetActor->CanceledDelegate.AddUObject(const_cast<UDFAT_WaitTargetData_ReusableTA*>(this), &UDFAT_WaitTargetData_ReusableTA::OnTargetDataCancelledCallback);
	}
}

void UDFAT_WaitTargetData_ReusableTA::FinalizeTargetActor(AGameplayAbilityTargetActor* InTargetActor) const
{
	check(InTargetActor);
	check(Ability);
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	// TODO: Add TA to ASC's SpawnedTargetActors array when it's spawned.
	//if (AbilitySystemComponent.IsValid())
	//{
	//	// User ability activation is inhibited while this is active
	//	AbilitySystemComponent->SpawnedTargetActors.Push(InTargetActor);
	//}

	InTargetActor->StartTargeting(Ability);

	if (InTargetActor->ShouldProduceTargetData())
	{
		// If instant confirm, then stop targeting immediately.
		// Note this is kind of bad: we should be able to just call a static func on the CDO to do this. 
		// But then we wouldn't get to set ExposeOnSpawnParameters.
		if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
		{
			InTargetActor->ConfirmTargeting();
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
			InTargetActor->BindToConfirmCancelInputs();
		}
	}
}

void UDFAT_WaitTargetData_ReusableTA::RegisterTargetDataCallbacks()
{
	check(IsValid(this));
	check(AbilitySystemComponent.Get())
	check(Ability);
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UDFAT_WaitTargetData_ReusableTA::OnTargetDataReplicatedCallback);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UDFAT_WaitTargetData_ReusableTA::OnTargetDataReplicatedCancelledCallback);

			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}

void UDFAT_WaitTargetData_ReusableTA::OnDestroy(bool AbilityEnded)
{
	DF_NETCUSTOMLOG(Ability->GetAvatarActorFromActorInfo(), LogDFGAS, Log, TEXT("Start"));

	if (TargetActor)
	{
		if (ADFGATA_Trace* DFTraceTargetActor = Cast<ADFGATA_Trace>(TargetActor))
		{
			DFTraceTargetActor->StopTargeting();
		}
		else
		{
			TargetActor->SetActorTickEnabled(false);

			TargetActor->TargetDataReadyDelegate.Clear();
			TargetActor->CanceledDelegate.Clear();

			if (AbilitySystemComponent.IsValid())
			{
				AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
				AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
				TargetActor->GenericDelegateBoundASC = nullptr;
			}
		}
	}

	Super::OnDestroy(AbilityEnded);
}

bool UDFAT_WaitTargetData_ReusableTA::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}
