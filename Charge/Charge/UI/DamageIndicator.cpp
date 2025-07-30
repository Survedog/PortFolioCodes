// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DamageIndicator.h"
#include "Components/WidgetComponent.h"

ADamageIndicator::ADamageIndicator() : TotalLifetime(5.0f), RemainingLifetime(TotalLifetime)
{
	PrimaryActorTick.bCanEverTick = true;

	DamageIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageIndicatorWidget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DamageIndicatorWidgetClassRef(TEXT("/Game/Charge/Blueprint/UI/WBP_DamageIndicator.WBP_DamageIndicator_C"));
	if (DamageIndicatorWidgetClassRef.Class)
	{
		DamageIndicatorWidget->SetWidgetClass(DamageIndicatorWidgetClassRef.Class);
	}

	RootComponent = DamageIndicatorWidget;
}

void ADamageIndicator::BeginPlay()
{
	Super::BeginPlay();

	RemainingLifetime = TotalLifetime;
}

void ADamageIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RemainingLifetime -= DeltaTime;
	if (RemainingLifetime <= 0.0f)
	{
		Destroy();
	}
}

