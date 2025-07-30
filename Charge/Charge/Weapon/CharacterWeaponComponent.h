// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterWeaponComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwapCurrentWeapon, uint32 /*NewCurrentWeaponIndex*/);
DECLARE_DELEGATE_OneParam(FOnWeaponInventoryChange, const TArray<TObjectPtr<class UWeapon>>& /*WeaponInventory*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHARGE_API UCharacterWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterWeaponComponent();

public:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	void OnOwnerASCSet(class UAbilitySystemComponent* TargetASC);

	UFUNCTION(BlueprintCallable)
	bool ObtainWeapon(const TSubclassOf<UWeapon>& NewWeaponClass, bool bSwapToNewWeapon = true);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "EditorFunc|Combat", DisplayName = "ObtainWeaponFromClass")
	void ObtainWeaponFromClass_EditorFunc();

	UFUNCTION(BlueprintCallable)
	void SwapCurrentWeapon(int32 NewCurrentWeaponIndex, class UAbilitySystemComponent* TargetASC, bool bIsNewWeapon = false);

	UFUNCTION(BlueprintCallable)
	bool DropWeapon(int32 DropWeaponIndex);

	FORCEINLINE bool DropCurrentWeapon() { return DropWeapon(CurrentWeaponIndex); }
	FORCEINLINE int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }
	FORCEINLINE class UWeapon* GetCurrentWeapon() const { return WeaponInventory[CurrentWeaponIndex]; }

	FORCEINLINE int32 GetMaxWeaponAmount() const { return MaxWeaponAmount; }
	FORCEINLINE class ACPCharacterBase* GetCPCharacterOwner() const { return CPCharacterOwner.Get(); }
	FORCEINLINE const TArray<TObjectPtr<class UWeapon>>& GetWeaponInventory() const { return WeaponInventory; }

	FOnWeaponInventoryChange OnWeaponInventoryChange;
	FOnSwapCurrentWeapon OnSwapCurrentWeapon;

protected:
	UFUNCTION()
	void OnRep_CurrentWeaponIndex();

	UFUNCTION()
	void OnRep_WeaponInventory();

	void ApplyWeaponMesh(const class UWeapon* InWeapon);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Character)
	TWeakObjectPtr<class ACPCharacterBase> CPCharacterOwner;

	const int32 MaxWeaponAmount = 2;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponIndex, VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	int32 CurrentWeaponIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EditorFunc|Combat", Meta = (ClampMin = 0))
	int32 WeaponToSwapIndex;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponInventory, EditAnywhere, BlueprintReadWrite, Category = Combat)
	TArray<TObjectPtr<class UWeapon>> WeaponInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TArray<TSubclassOf<class UWeapon>> InitialWeaponClassess;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitWeaponStatEffect;

	UPROPERTY(EditAnywhere, Category = "EditorFunc|Combat")
	TSubclassOf<class UWeapon> NewWeaponClass_ForEditorFunc;
};
