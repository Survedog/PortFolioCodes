// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "InteractTriggerComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Interact Trigger", BlueprintSpawnableComponent))
class CHARGE_API UInteractTriggerComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:
	UInteractTriggerComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentBeginOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UI)
//	TObjectPtr<class UWidgetComponent> OwnerWidgetComponent;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
//	TObjectPtr<class UUserWidget> InteractButtonWidget;
};
