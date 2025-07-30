// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CPCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/CPCharacterWidgetInterface.h"
#include "Interface/CPFireProjectileInterface.h"
#include "AbilitySystemInterface.h"
#include "CPPlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBindingInput);

/**
 * 
 */
UCLASS()
class CHARGE_API ACPPlayerCharacter : public ACPCharacterBase, public ICPCharacterWidgetInterface, public ICPFireProjectileInterface
{
	GENERATED_BODY()
	
public:
	ACPPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void MulticastRPCSetDead_Implementation() override;

// GAS
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	FORCEINLINE const TMap<TSubclassOf<class UGameplayAbility>, TObjectPtr<class UInputAction>>& GetParamAbilityInputMap() const { return ParamAbilityInputMap; }

public:
	FOnBindingInput OnBindingInput;

protected:
	void AbilityInputPressed(int32 InputID);
	void AbilityInputReleased(int32 InputID);
	void ApplyMoveSpeedAttribute(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> InputAbilityMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TMap<TSubclassOf<class UGameplayAbility>, TObjectPtr<class UInputAction>> ParamAbilityInputMap;

// Camera
public:
	FORCEINLINE TObjectPtr<class USpringArmComponent> GetCameraBoom() { return CameraBoom; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

// Character Control
public:
	FORCEINLINE const class UCPCharacterControlData* GetIdleModeControlData() const { return IdleModeControlData; }
	FORCEINLINE const class UCPCharacterControlData* GetAimingModeControlData() const { return AimingModeControlData; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterControl)
	TObjectPtr<class UCPCharacterControlData> IdleModeControlData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterControl)
	TObjectPtr<class UCPCharacterControlData> AimingModeControlData;

// Input
public:
	FORCEINLINE float GetAimRotationPitch() const { return AimRotationPitch; };
	
	virtual void SetIgnoreMovement(bool bIgnore) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChargeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SwapWeaponButtonAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SwapWeaponWheelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DropWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	float AimRotationPitch;

	void StartMoving();
	void Move(const FInputActionValue& Value);
	void OnMoveEnd();
	
	void Look(const FInputActionValue& Value);	

	UFUNCTION(Server, Unreliable)
	void ServerRPCChangeCameraPitch(float InPitch);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCChangeCameraPitch(float InPitch);

// Combat
public:	
	virtual FVector GetProjectileSpawnLocation() const override;
	virtual FVector GetProjectileTargetLocation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float FireTraceStartDistance;

// Animation
public:
	FORCEINLINE class UAnimMontage* GetRollMontage() { return RollMontage; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<UAnimMontage> RollMontage;


// UI
public:
	FORCEINLINE virtual void SetupHUDWidget(class UCPHUDWidget* InHUDWidget) override { HUDWidget = InHUDWidget; }
	FORCEINLINE void SetCrosshairVisibility(bool bInIsVisible);

protected:
	TObjectPtr<class UCPHUDWidget> HUDWidget;

// Interaction System
public:
	FORCEINLINE class ICPInteractiveObjectInterface* GetInteractTarget() const { return InteractTarget.Get(); }
	void SetInteractTarget(class ICPInteractiveObjectInterface* InInteractTarget);
	void ClearInteractTarget(class ICPInteractiveObjectInterface* InInteractTarget);

protected:
	TWeakInterfacePtr<class ICPInteractiveObjectInterface> InteractTarget;
};
