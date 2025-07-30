// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_EnableGettingUp.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GA/GA_KnockDown.h"
#include "Charge.h"

UAnimNotify_EnableGettingUp::UAnimNotify_EnableGettingUp()
{
	static ConstructorHelpers::FClassFinder<UGameplayAbility> KnockDownAbilityClassRef(TEXT("/Game/Charge/Blueprint/GA/BPGA_KnockedDown.BPGA_KnockedDown_C"));
	if (KnockDownAbilityClassRef.Class)
	{
		KnockDownAbilityClass = KnockDownAbilityClassRef.Class;
	}
}

void UAnimNotify_EnableGettingUp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	CP_NETCUSTOMLOG(MeshComp->GetOwner(), LogCP, Log, TEXT("Start"));

	IAbilitySystemInterface* GASInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (!GASInterface)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GASInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(KnockDownAbilityClass);
	if (!AbilitySpec)
	{
		return;
	}

	UGA_KnockDown* KnockDownAbility = CastChecked<UGA_KnockDown>(AbilitySpec->GetPrimaryInstance());
	KnockDownAbility->EnableGettingUp();
}
