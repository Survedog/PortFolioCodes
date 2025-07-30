// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CPInteractiveObjectInterface.h"
#include "WeaponItem.generated.h"

UCLASS()
class CHARGE_API AWeaponItem : public AActor, public ICPInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponItem();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void SetWeaponClass(TSubclassOf<class UWeapon> NewWeaponClass);

protected:
	virtual void OnInteract_Implementation(class ACPPlayerCharacter* InteractedCharacter) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void ChangeWeaponMesh(TSoftObjectPtr<UStaticMesh> InWeaponMesh);
	FORCEINLINE virtual UStaticMeshComponent* GetMeshComponent() const override { return ItemMesh; }

	UFUNCTION()
	void OnRep_WeaponClass();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponClass, EditAnywhere, BlueprintReadWrite, Category = Item)
	TSubclassOf<class UWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<class UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger)
	TObjectPtr<class UInteractTriggerComponent> InteractTrigger;
};
