// Copyright 2024, Survedog. All rights reserved.


#include "Structure/DFStructureBase.h"
#include "GameFramework/PlayerController.h"
#include "Player/DFPlayerPawn.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attribute/DFHealthAttributeSet.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "GameplayEffect.h"
#include "GAS/DFGameplayTags.h"
#include "DFLog.h"

ADFStructureBase::ADFStructureBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->ReplicationMode = EGameplayEffectReplicationMode::Full;

	HealthAttributeSet = CreateDefaultSubobject<UDFHealthAttributeSet>(TEXT("HealthAttributeSet"));

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageEffectClassRef(TEXT("/Game/DefenseForce/Blueprint/GAS/GE/BPGE_AttackDamage.BPGE_AttackDamage_C"));
	if (DamageEffectClassRef.Class)
	{
		DamageEffectClass = DamageEffectClassRef.Class;
	}

	BuildCost = 0.0f;
}

float ADFStructureBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	IDFAttackerInfoInterface* AttackerInfoInterface = Cast<IDFAttackerInfoInterface>(EventInstigator);
	if (AttackerInfoInterface)
	{
		UAbilitySystemComponent* AttackerASC = AttackerInfoInterface->GetAttackerActorASC();
		if (AttackerASC)
		{
			FGameplayEffectContextHandle EffectContextHandle = AttackerASC->MakeEffectContext();
			EffectContextHandle.AddInstigator(AttackerASC->GetOwnerActor(), DamageCauser);

			FGameplayEffectSpecHandle EffectSpecHandle = AttackerASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, EffectContextHandle);
			if (EffectSpecHandle.IsValid())
			{
				EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GASTAG_Attribute_Health_DamageToApply, DamageAmount);
				AttackerASC->BP_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, ASC);
			}
		}
	}
	return DamageAmount;
}

void ADFStructureBase::AbilityInputPressed(EDFAbilityInputID InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	if (Spec)
	{
		Spec->InputPressed = true;

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle, true);
		}
	}
}

void ADFStructureBase::AbilityInputReleased(EDFAbilityInputID InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

bool ADFStructureBase::TryActivateAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(InAbilityClass);
	if (Spec && !Spec->IsActive())
	{
		return ASC->TryActivateAbility(Spec->Handle, bAllowRemoteActivation);
	}
	return false;
}

int32 ADFStructureBase::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	return ASC->HandleGameplayEvent(EventTag, &Payload);
}

void ADFStructureBase::CancelAbilityOfClass(TSubclassOf<class UGameplayAbility> InAbilityClass)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(InAbilityClass);
	if (Spec && Spec->IsActive())
	{
		ASC->CancelAbilityHandle(Spec->Handle);
	}
}

float ADFStructureBase::GetHp() const
{
	return HealthAttributeSet->GetHp();
}

void ADFStructureBase::SetHp(float NewHp)
{
	HealthAttributeSet->SetHp(NewHp);
}

float ADFStructureBase::GetMaxHp() const
{
	return HealthAttributeSet->GetMaxHp();
}

void ADFStructureBase::SetMaxHp(float NewMaxHp)
{
	HealthAttributeSet->SetMaxHp(NewMaxHp);
}

void ADFStructureBase::BeginPlay()
{
	Super::BeginPlay();

	DF_NETLOG(LogDF, Log, TEXT("Start"));	
	if (APlayerController* LocalPlayer = GEngine->GetFirstLocalPlayerController(GetWorld()))
	{
		if (ADFPlayerPawn* DFLocalPlayerPawn = Cast<ADFPlayerPawn>(LocalPlayer->GetPawn()))
		{
			OnBeginCursorOver.AddDynamic(DFLocalPlayerPawn, &ADFPlayerPawn::OnBeginCursorOverStructureCallback);
			OnEndCursorOver.AddDynamic(DFLocalPlayerPawn, &ADFPlayerPawn::OnEndCursorOverStructureCallback);
		}
	}

	ASC->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		HealthAttributeSet->OnHpZero.AddDynamic(this, &ADFStructureBase::OnDestructedCallback);

		// Give Abilities
		for (auto Ability : NonInputAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(Ability);
			ASC->GiveAbility(AbilitySpec);
		}

		for (auto InputAbilityPair : InputAbilityMap)
		{
			FGameplayAbilitySpec AbilitySpec(InputAbilityPair.Value);
			AbilitySpec.InputID = static_cast<int32>(InputAbilityPair.Key);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void ADFStructureBase::OnDestructedCallback_Implementation()
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	OnStructureDestructed.Broadcast();
}
