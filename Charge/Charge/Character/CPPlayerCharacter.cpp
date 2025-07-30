// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CPPlayerCharacter.h"
#include "Player/CPPlayerController.h"
#include "UI/CPHUDWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/CPPlayerAnimInstance.h"
#include "Character/Data/CPCharacterControlData.h"
#include "Weapon/CharacterWeaponComponent.h"
#include "Player/CPPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GASTag/CPGameplayTag.h"
#include "Attribute/CPChargeAttributeSet.h"
#include "Attribute/CPCharacterBaseAttributeSet.h"
#include "Physics/CPCollision.h"
#include "Net/UnrealNetwork.h"
#include "Interface/CPInteractiveObjectInterface.h"
#include "Charge.h"

ACPPlayerCharacter::ACPPlayerCharacter() : AimRotationPitch(0.0f)
{
	// Movement
	GetCharacterMovement()->JumpZVelocity = 460.0f;

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/StoreAssets/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->SetRelativeLocation(FVector(0.0, 0.0, 30.0));
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Charge/Input/IMC_Default.IMC_Default'"));
	if (InputMappingContextRef.Object)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Move.IA_Move'"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Look.IA_Look'"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SprintActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Sprint.IA_Sprint'"));
	if (SprintActionRef.Object)
	{
		SprintAction = SprintActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Object)
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ChargeActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Charge.IA_Charge'"));
	if (ChargeActionRef.Object)
	{
		ChargeAction = ChargeActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AimActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Aim.IA_Aim'"));
	if (AimActionRef.Object)
	{
		AimAction = AimActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RollActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Roll.IA_Roll'"));
	if (RollActionRef.Object)
	{
		RollAction = RollActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SwapWeaponButtonActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_SwapWeapon_Button.IA_SwapWeapon_Button'"));
	if (SwapWeaponButtonActionRef.Object)
	{
		SwapWeaponButtonAction = SwapWeaponButtonActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SwapWeaponWheelActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_SwapWeapon_Wheel.IA_SwapWeapon_Wheel'"));
	if (SwapWeaponWheelActionRef.Object)
	{
		SwapWeaponWheelAction = SwapWeaponWheelActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DropWeaponActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_DropWeapon.IA_DropWeapon'"));
	if (DropWeaponActionRef.Object)
	{
		DropWeaponAction = DropWeaponActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Charge/Input/Actions/IA_Interact.IA_Interact'"));
	if (InteractActionRef.Object)
	{
		InteractAction = InteractActionRef.Object;
	}

	// Character Control
	static ConstructorHelpers::FObjectFinder<UCPCharacterControlData> IdleModeControlDataRef(TEXT("/Script/Charge.CPCharacterControlData'/Game/Charge/Blueprint/GameData/ControlData/DA_CharacterControl_Idle.DA_CharacterControl_Idle'"));
	if (IdleModeControlDataRef.Object)
	{
		IdleModeControlData = IdleModeControlDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCPCharacterControlData> AimingModeControlDataRef(TEXT("/Script/Charge.CPCharacterControlData'/Game/Charge/Blueprint/GameData/ControlData/DA_CharacterControl_Aim.DA_CharacterControl_Aim'"));
	if (AimingModeControlDataRef.Object)
	{
		AimingModeControlData = AimingModeControlDataRef.Object;
	}

	CameraBoom->bDoCollisionTest = IdleModeControlData->bDoCollisionTest;
	CameraBoom->TargetArmLength = IdleModeControlData->TargetArmLength;
	CameraBoom->SocketOffset = IdleModeControlData->SocketOffset;

	bUseControllerRotationYaw = IdleModeControlData->bUseControllerRotationYaw;
	GetCharacterMovement()->bOrientRotationToMovement = IdleModeControlData->bOrientRotationToMovement;

	// Animation
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Charge/Animation/Player/ABP_CPPlayerCharacter.ABP_CPPlayerCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Charge/Animation/Player/Common/AM_Player_Roll.AM_Player_Roll''"));
	if (RollMontageRef.Object)
	{
		RollMontage = RollMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> StunnedMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Charge/Animation/Player/Rifle/AM_Player_Stunned_Rifle.AM_Player_Stunned_Rifle'"));
	if (StunnedMontageRef.Object)
	{
		StunnedMontage = StunnedMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> KnockedDownMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Charge/Animation/Player/AM_Player_KnockedDown.AM_Player_KnockedDown'"));
	if (KnockedDownMontageRef.Object)
	{
		KnockedDownMontage = KnockedDownMontageRef.Object;
	}

	// Combat
	FireTraceStartDistance = 250.0f;
}

void ACPPlayerCharacter::BeginPlay()
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::BeginPlay();

	// Input Mapping Context
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void ACPPlayerCharacter::PossessedBy(AController* NewController)
{
	CP_NETLOG(LogCP, Log, TEXT("Start"));
	Super::PossessedBy(NewController);

	ACPPlayerState* CPPlayerState = GetPlayerState<ACPPlayerState>();
	if (CPPlayerState)
	{
		ASC = CPPlayerState->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(CPPlayerState, this);

		// Init objects that need ASC
		WeaponComp->OnOwnerASCSet(ASC);
		UCPPlayerAnimInstance* CPPlayerAnimInstance = Cast<UCPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (CPPlayerAnimInstance)
		{
			CPPlayerAnimInstance->SetCharacterASC(ASC);
		}

		// Give Abilities
		for (auto NonInputAbility : NonInputAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(NonInputAbility);
			ASC->GiveAbility(AbilitySpec);
		}

		for (auto InputAbilityPair : InputAbilityMap)
		{
			FGameplayAbilitySpec AbilitySpec(InputAbilityPair.Value);
			AbilitySpec.InputID = InputAbilityPair.Key;
			ASC->GiveAbility(AbilitySpec);
		}

		for (auto AbilityInputPair : ParamAbilityInputMap)
		{
			const TSubclassOf<UGameplayAbility>& AbilityClass = AbilityInputPair.Key;
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			ASC->GiveAbility(AbilitySpec);
		}

		// Init Stat
		ASC->BP_ApplyGameplayEffectToSelf(InitCharacterStatEffect, 1.0f, FGameplayEffectContextHandle());

		// TODO: Move speed change logic into custom character movement component.
		const UCPCharacterBaseAttributeSet* CharacterAttributeSet = ASC->GetSet<UCPCharacterBaseAttributeSet>();
		CharacterAttributeSet->OnHpZero.AddDynamic(this, &ACPPlayerCharacter::MulticastRPCSetDead);
		GetCharacterMovement()->MaxWalkSpeed = CharacterAttributeSet->GetMoveSpeed();
		ASC->GetGameplayAttributeValueChangeDelegate(CharacterAttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &ACPPlayerCharacter::ApplyMoveSpeedAttribute);
	}

	// Debug
	//APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
	//PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void ACPPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ACPPlayerState* CPPlayerState = GetPlayerState<ACPPlayerState>();
	if (CPPlayerState)
	{
		ASC = CPPlayerState->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(CPPlayerState, this);

		WeaponComp->OnOwnerASCSet(ASC);
		UCPPlayerAnimInstance* CPPlayerAnimInstance = Cast<UCPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (CPPlayerAnimInstance)
		{
			CPPlayerAnimInstance->SetCharacterASC(ASC);
		}

		const UCPCharacterBaseAttributeSet* CharacterAttributeSet = ASC->GetSet<UCPCharacterBaseAttributeSet>();
		CharacterAttributeSet->OnHpZero.AddDynamic(this, &ACPPlayerCharacter::MulticastRPCSetDead);

		GetCharacterMovement()->MaxWalkSpeed = CharacterAttributeSet->GetMoveSpeed();
		ASC->GetGameplayAttributeValueChangeDelegate(CharacterAttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &ACPPlayerCharacter::ApplyMoveSpeedAttribute);
	}
}

void ACPPlayerCharacter::MulticastRPCSetDead_Implementation()
{
	Super::MulticastRPCSetDead_Implementation();

	CP_NETLOG(LogCPNET, Log, TEXT("Start"));
	CameraBoom->bDoCollisionTest = false;
}

UAbilitySystemComponent* ACPPlayerCharacter::GetAbilitySystemComponent() const
{
	if (ASC == nullptr)
	{
		CP_NETLOG(LogCP, Warning, TEXT("Couldn't get ASC from character."));
	}
	return ASC;
}

void ACPPlayerCharacter::AbilityInputPressed(int32 InputID)
{
	//CP_NETLOG(LogCP, Log, TEXT("InputID: %d"), InputID);
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
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

void ACPPlayerCharacter::AbilityInputReleased(int32 InputID)
{
	//CP_NETLOG(LogCP, Log, TEXT("InputID: %d"), InputID);
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	if (Spec)
	{
		Spec->InputPressed = false;

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void ACPPlayerCharacter::ApplyMoveSpeedAttribute(const FOnAttributeChangeData& ChangeData)
{
	GetCharacterMovement()->MaxWalkSpeed = ChangeData.NewValue;
}

void ACPPlayerCharacter::SetIgnoreMovement(bool bIgnore)
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(bIgnore);
	}
}

void ACPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (IsLocallyControlled())
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ACPPlayerCharacter::StartMoving);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACPPlayerCharacter::OnMoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::AbilityInputPressed, 1);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACPPlayerCharacter::AbilityInputReleased, 1);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::AbilityInputPressed, 2);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACPPlayerCharacter::AbilityInputReleased, 2);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::AbilityInputPressed, 3);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ACPPlayerCharacter::AbilityInputPressed, 4);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACPPlayerCharacter::AbilityInputReleased, 4);

		EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Started, this, &ACPPlayerCharacter::AbilityInputPressed, 5);
		EnhancedInputComponent->BindAction(ChargeAction, ETriggerEvent::Completed, this, &ACPPlayerCharacter::AbilityInputReleased, 5);

		// Input ID 6 is for GA_Move
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACPPlayerCharacter::AbilityInputPressed, 7);

		EnhancedInputComponent->BindAction(DropWeaponAction, ETriggerEvent::Started, this, &ACPPlayerCharacter::AbilityInputPressed, 8);

		OnBindingInput.Broadcast();
	}
}

void ACPPlayerCharacter::StartMoving()
{
	AbilityInputPressed(6);
}

void ACPPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementInput = Value.Get<FVector2D>();

	const FRotator YawRotation = FRotator(0.0, Controller->GetControlRotation().Yaw, 0.0);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, MovementInput.X);
	AddMovementInput(Right, MovementInput.Y);
}

void ACPPlayerCharacter::OnMoveEnd()
{
	AbilityInputReleased(6);
}

void ACPPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);

	ServerRPCChangeCameraPitch(Camera->GetComponentRotation().Pitch);
}

void ACPPlayerCharacter::ServerRPCChangeCameraPitch_Implementation(float InPitch)
{
	MulticastRPCChangeCameraPitch(InPitch);
}

void ACPPlayerCharacter::MulticastRPCChangeCameraPitch_Implementation(float InPitch)
{
	AimRotationPitch = InPitch;
}

FVector ACPPlayerCharacter::GetProjectileSpawnLocation() const
{
	static FName MuzzleSocketName("MuzzleSocket");
	return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

FVector ACPPlayerCharacter::GetProjectileTargetLocation() const
{
	FVector TargetLocation;
	const float AttackRange = 10000.0f;

	bool bIsAiming = ASC->HasMatchingGameplayTag(GASTAG_CHARACTER_ACTION_Aim);
	if (bIsAiming)
	{
		const FVector ControlVector = GetControlRotation().Vector();
		const FVector TraceStart = Camera->GetComponentLocation() + ControlVector * FireTraceStartDistance;
		const FVector TraceEnd = TraceStart + ControlVector * AttackRange;

		FHitResult OutHitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, CCHANNEL_PROJECTILE_DEST, CollisionQueryParams);
		if (bIsHit)
		{
			CP_NETLOG(LogCP, Log, TEXT("HitActor: %s"), *OutHitResult.GetActor()->GetName());
			TargetLocation = OutHitResult.ImpactPoint;
		}
		else
		{
			TargetLocation = TraceEnd;
		}
#if ENABLE_DRAW_DEBUG
		//FColor DebugDrawColor = bIsHit ? FColor::Green : FColor::Red;
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugDrawColor, false, 1.0f);
		//DrawDebugSphere(GetWorld(), TargetLocation, 20.0f, 8, FColor::Blue, false, 1.0f);
#endif
	}
	else
	{
		TargetLocation = GetProjectileSpawnLocation() + GetActorRotation().Vector() * AttackRange;
	}
	return TargetLocation;
}

//UClass* ACPPlayerCharacter::GetProjectileClass() const
//{
//	return Weapon->GetProjectileClass();
//}

void ACPPlayerCharacter::SetCrosshairVisibility(bool bInIsVisible)
{
	if (HUDWidget)
	{
		HUDWidget->SetCrosshairVisibility(bInIsVisible);
	}
}

void ACPPlayerCharacter::SetInteractTarget(ICPInteractiveObjectInterface* InInteractTarget)
{
	if (InteractTarget.IsValid())
	{
		ClearInteractTarget(InteractTarget.Get());
	}

	InteractTarget = InInteractTarget;
	if (IsLocallyControlled())
	{
		InteractTarget->HighlightObjectOutline();
	}
}

void ACPPlayerCharacter::ClearInteractTarget(ICPInteractiveObjectInterface* InInteractTarget)
{
	if (InteractTarget == InInteractTarget)
	{
		InteractTarget.Reset();
		if (IsLocallyControlled())
		{
			InInteractTarget->UnhighlightObjectOutline();
		}
	}
}