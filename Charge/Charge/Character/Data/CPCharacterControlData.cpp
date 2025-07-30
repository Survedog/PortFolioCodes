// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Data/CPCharacterControlData.h"

UCPCharacterControlData::UCPCharacterControlData()
{
	TargetArmLength = 400.0f;
	SocketOffset = FVector::Zero();
	bDoCollisionTest = true;
}
