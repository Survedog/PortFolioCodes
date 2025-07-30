// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GASUserWidget.h"
#include "AbilitySystemComponent.h"

void UGASUserWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}

UAbilitySystemComponent* UGASUserWidget::GetAbilitySystemComponent() const
{
    return ASC;
}
