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
#include "Kismet/GameplayStatics.h"

#include "SC2AIPlayerController.h"

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

	SC2AIComponent = CreateDefaultSubobject<USC2AIComponent>(TEXT("SC2AIComponent"));
	SC2AIComponent->SetupAttachment(RootComponent);
	SC2AIComponent->SetGroup(EGroup::Enemy);
}

void ASC2AICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASC2AICharacter::SetGroup(EGroup GroupType)
{
	if (SC2AIComponent)
	{
		SC2AIComponent->SetGroup(GroupType);
	}
}

void ASC2AICharacter::GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount, int& BigLeftCount, int& BigRightCount)
{
	if (SC2AIComponent)
	{
		SC2AIComponent->GetOverlapCount(FwdCount, LeftCount, RightCount, FwdLeftCount, FwdRightCount, BigLeftCount, BigRightCount);
	}
}

void ASC2AICharacter::SetCollisionVisible(bool IsVisible)
{
	if (SC2AIComponent)
	{
		SC2AIComponent->SetCollisionVisible(IsVisible);
	}
}

void ASC2AICharacter::SetDestDirection(const FVector& Direction)
{
	if (SC2AIComponent)
	{
		SC2AIComponent->SetDestDirection(Direction);
	}
}

void ASC2AICharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (ASC2AIPlayerController* Controller = Cast<ASC2AIPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (Controller->GetSelectedCharacter() == this)
		{
			Controller->ResetSelectedCharacter();
		}
	}
}