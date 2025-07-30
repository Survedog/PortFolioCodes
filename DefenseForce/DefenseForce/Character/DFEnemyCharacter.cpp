// Copyright 2024, Survedog. All rights reserved.


#include "Character/DFEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attribute/DFCharacterAttributeSet.h"
#include "GAS/Attribute/DFHealthAttributeSet.h"
#include "Interface/DFAttackerInfoInterface.h"
#include "GAS/DFGameplayTags.h"
#include "AI/DFAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "DFLog.h"

ADFEnemyCharacter::ADFEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	DropMoneyAmount = 100.0f;
	bIsDead = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	CharacterAttributeSet = CreateDefaultSubobject<UDFCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	HealthAttributeSet = CreateDefaultSubobject<UDFHealthAttributeSet>(TEXT("HealthAttributeSet"));

	static ConstructorHelpers::FClassFinder<AController> AIControllerClassRef(TEXT("/Game/DefenseForce/Blueprint/Character/Enemy/BP_DFAIController.BP_DFAIController_C"));
	if (AIControllerClassRef.Class)
	{
		AIControllerClass = AIControllerClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageEffectClassRef(TEXT("/Game/DefenseForce/Blueprint/GAS/GE/BPGE_AttackDamage.BPGE_AttackDamage_C"));
	if (DamageEffectClassRef.Class)
	{
		DamageEffectClass = DamageEffectClassRef.Class;
	}
}

float ADFEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

	return 0.0f;
}

void ADFEnemyCharacter::AttackHitCheck_Implementation()
{
	AActor* TargetActor = GetAttackTargetActor();
	if (TargetActor)
	{
		const float DamageAmount = GetDamageAmount();
		TargetActor->TakeDamage(DamageAmount, FDamageEvent(), GetController(), this);
	}
}

UAbilitySystemComponent* ADFEnemyCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

#pragma region AttributeGetterSetters
float ADFEnemyCharacter::GetHp() const
{
	return HealthAttributeSet->GetHp();
}

void ADFEnemyCharacter::SetHp(float NewHp)
{
	HealthAttributeSet->SetHp(NewHp);
}

float ADFEnemyCharacter::GetMaxHp() const
{
	return HealthAttributeSet->GetMaxHp();
}

void ADFEnemyCharacter::SetMaxHp(float NewMaxHp)
{
	HealthAttributeSet->SetMaxHp(NewMaxHp);
}

float ADFEnemyCharacter::GetMoveSpeed() const
{
	return CharacterAttributeSet->GetMoveSpeed();
}

void ADFEnemyCharacter::SetMoveSpeed(float NewMoveSpeed)
{
	CharacterAttributeSet->SetMoveSpeed(NewMoveSpeed);
}

#pragma endregion

void ADFEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	ASC->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		HealthAttributeSet->OnHpZero.AddDynamic(this, &ADFEnemyCharacter::OnDead);

		for (auto Ability : InnateAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(Ability);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void ADFEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ADFEnemyCharacter, bIsDead, COND_None, REPNOTIFY_OnChanged);
}

AActor* ADFEnemyCharacter::GetAttackTargetActor() const
{
	if (HasAuthority())
	{
		ADFAIController* DFAIController = CastChecked<ADFAIController>(GetController());
		UBlackboardComponent* BBComp = DFAIController->GetBlackboardComponent();
		if (BBComp)
		{
			UObject* BBKeyObject = BBComp->GetValueAsObject(TEXT("TargetActor"));
			if (BBKeyObject)
			{
				return Cast<AActor>(BBKeyObject);
			}
		}
	}
	return nullptr;
}

void ADFEnemyCharacter::OnDead_Implementation()
{
	DF_NETLOG(LogDF, Log, TEXT("Start"));
	bIsDead = true;
	ASC->AddLooseGameplayTag(GASTAG_Character_State_Dead);
}

void ADFEnemyCharacter::OnRep_bIsDead()
{
	OnDead();
}