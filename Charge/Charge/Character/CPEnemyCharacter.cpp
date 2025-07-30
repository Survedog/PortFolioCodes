// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CPEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "Attribute/CPWeaponAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/CPCharacterAnimInstance.h"
#include "AI/CPEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Charge.h"

ACPEnemyCharacter::ACPEnemyCharacter()
{
	/* Initialization */
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));	
	CharacterBaseAttributeSet = CreateDefaultSubobject<UCPCharacterBaseAttributeSet>(TEXT("CharacterBaseAttributeSet"));
	WeaponAttributeSet = CreateDefaultSubobject<UCPWeaponAttributeSet>(TEXT("WeaponAttributeSet"));

	AIControllerClass = ACPEnemyAIController::StaticClass();

	/* Load Assets */
	// Mesh Assets
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/StoreAssets/ParagonGrux/Characters/Heroes/Grux/Meshes/Grux.Grux'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Animation Assets
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Charge/Animation/Enemy/Grux/ABP_Grux.ABP_Grux_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// AI Assets
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/Charge/Blueprint/AI/BT_Enemy_Grux.BT_Enemy_Grux'"));
	if (BehaviorTreeAssetRef.Object)
	{
		BehaviorTreeAsset = BehaviorTreeAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackBoardAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/Charge/Blueprint/AI/BB_Enemy.BB_Enemy'"));
	if (BlackBoardAssetRef.Object)
	{
		BlackBoardAsset = BlackBoardAssetRef.Object;
	}
}

void ACPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	 
	ASC->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		// Give Abilities
		for (auto NonInputAbility : NonInputAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(NonInputAbility);
			ASC->GiveAbility(AbilitySpec);
		}

		// Init Stat
		ASC->BP_ApplyGameplayEffectToSelf(InitCharacterStatEffect, 1.0f, FGameplayEffectContextHandle());
		WeaponComp->OnOwnerASCSet(ASC);

		CharacterBaseAttributeSet->OnHpZero.AddDynamic(this, &ACPEnemyCharacter::MulticastRPCSetDead);
		GetCharacterMovement()->MaxWalkSpeed = CharacterBaseAttributeSet->GetMoveSpeed();
	}

	UCPCharacterAnimInstance* CPCharacterAnimInstance = Cast<UCPCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (CPCharacterAnimInstance)
	{
		CPCharacterAnimInstance->SetCharacterASC(ASC);
	}
}

void ACPEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CPEnemyAIController = CastChecked<ACPEnemyAIController>(NewController);
}

float ACPEnemyCharacter::TakeDamage(float InDamageAmount, float InStunValue, const FHitResult& InHitResult, const FGameplayTagContainer& InDamageReactionAbilityTags, AController* InDamageInstigatorController, AActor* InDamageCauser, const TSubclassOf<UDamageType> InDamageTypeClass)
{
	Super::TakeDamage(InDamageAmount, InStunValue, InHitResult, InDamageReactionAbilityTags, InDamageInstigatorController, InDamageCauser, InDamageTypeClass);

	if (InDamageInstigatorController && InDamageInstigatorController->IsPlayerController())
	{
		AttackTarget = InDamageInstigatorController->GetPawn();
		SetBBComponentAttackTarget(InDamageInstigatorController->GetPawn());
	}
	return InDamageAmount;
}

void ACPEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPEnemyCharacter, AttackTarget);
}

UAbilitySystemComponent* ACPEnemyCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ACPEnemyCharacter::MulticastRPCSetDead_Implementation()
{
	Super::MulticastRPCSetDead_Implementation();
	
	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	if (CPEnemyAIController)
	{
		CPEnemyAIController->StopAI();
	}

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ACPEnemyCharacter::K2_DestroyActor, 5.0f, false);
}

FVector ACPEnemyCharacter::GetProjectileSpawnLocation() const
{
	return GetActorLocation();
}

FVector ACPEnemyCharacter::GetProjectileTargetLocation() const
{
	const float AttackRange = 10000.0f;
	if (AttackTarget.IsValid())
	{
		return AttackTarget->GetTargetLocation();
	}
	return GetActorLocation() + GetActorForwardVector() * AttackRange;
}

void ACPEnemyCharacter::SetIgnoreMovement(bool bIgnore)
{
	if (CPEnemyAIController)
	{
		CPEnemyAIController->SetIgnoreMoveInput(bIgnore);
		CPEnemyAIController->SetIgnoreLookInput(bIgnore);
	}
}

void ACPEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == TEXT("AttackTarget"))
	{
		SetBBComponentAttackTarget(AttackTarget.Get());
	}
}

void ACPEnemyCharacter::SetBBComponentAttackTarget(APawn* InAttackTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
		BBComponent->SetValueAsObject(TEXT("Target"), InAttackTarget);
	}
}
