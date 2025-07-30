// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageIndicator.generated.h"

UCLASS(Abstract, Blueprintable)
class CHARGE_API ADamageIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageIndicator();

	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageAmount(float InDamageAmount);

protected:
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> DamageIndicatorWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TotalLifetime;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float RemainingLifetime;
};
