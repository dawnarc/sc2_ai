// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SC2AICharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ASC2AICharacter::ASC2AICharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1600.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Group = EGroup::Enemy;
}

void ASC2AICharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (EGroup::Enemy == Group)
	{
		return;
	}

	CalcMovement(DeltaSeconds);

	LerpRotate(DeltaSeconds);

	RotateCollisionForward(DeltaSeconds);
}

void ASC2AICharacter::SetGroup(EGroup GroupType)
{
	Group = GroupType;
}

FVector ASC2AICharacter::CalcLessestDirection(TMap<int32, FVector>& DirectionMap)
{
	int Lessest = 999999;

	FVector Ret;

	for (auto& Kvp : DirectionMap)
	{
		//UE_LOG(LogCategory, Log, TEXT("Key: %s, Value: %d"), Kvp.Key, *Kvp.Value);
		if (Kvp.Key < Lessest)
		{
			Lessest = Kvp.Key;
			Ret = Kvp.Value;
		}
	}

	return Ret;
}