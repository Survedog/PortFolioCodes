// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "GASUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHARGE_API UGASUserWidget : public UUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* NewASC);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
