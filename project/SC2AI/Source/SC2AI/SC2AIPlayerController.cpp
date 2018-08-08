// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SC2AIPlayerController.h"
#include "NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "SC2AICharacter.h"
#include "Engine/World.h"
#include "Classes/GameFramework/SpectatorPawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/TextBlock.h"

#include "SC2AIGameMode.h"
#include "DebugWidget.h"
#include "SC2AICharacter.h"


ASC2AIPlayerController::ASC2AIPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	SelectedCharacter = nullptr;
	DebugWidget = nullptr;
}

void ASC2AIPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

	if (SelectedCharacter && SelectedCharacter->IsValidLowLevelFast() && DebugWidget)
	{
		if (DebugWidget->TxtDebug)
		{
			int FwdCount = -1;
			int LeftCount = -1;
			int RightCount = -1;
			int FwdLeftCount = -1;
			int FwdRightCount = -1;

			SelectedCharacter->GetOverlapCount(FwdCount, LeftCount, RightCount, FwdLeftCount, FwdRightCount);
			FString Text = FString::Printf(TEXT("F:%d\t\tL:%d\t\tR:%d\t\tFL:%d\t\tFR:%d"), FwdCount, LeftCount, RightCount, FwdLeftCount, FwdRightCount);
			DebugWidget->TxtDebug->SetText(FText::FromString(Text));

			if (USkeletalMeshComponent* MeshComp = SelectedCharacter->GetMesh())
			{
				if (DebugHighlightMaterial)
				{
					MeshComp->SetMaterial(0, DebugHighlightMaterial);
				}
			}
		}
	}
}

void ASC2AIPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	/*InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASC2AIPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASC2AIPlayerController::OnSetDestinationReleased);*/

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASC2AIPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASC2AIPlayerController::MoveToTouchLocation);

	//Êó±êµã»÷
	InputComponent->BindAction("MouseClick", IE_Pressed, this, &ASC2AIPlayerController::OnMouseClick);
}

void ASC2AIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//if (!GetSpectatorPawn())
	//{
	//	ASpectatorPawn* SpectatorPawn = SpawnSpectatorPawn();
	//	SetSpectatorPawn(SpectatorPawn);
	//	//SpectatorPawn->SetActorRotation(FRotator(0.f, 0.f, 90.f));
	//}

	if (GetPawn())
	{
		GetPawn()->SetActorRotation(FRotator(-90.f, 0.f, 0.f));
	}

	if (DebugWidgetClass)
	{
		DebugWidget = CreateWidget<UDebugWidget>(this, DebugWidgetClass);
		if (DebugWidget)
		{
			DebugWidget->AddToViewport();
		}
	}
}

void ASC2AIPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		//SetNewMoveDestination(Hit.ImpactPoint);
		if (ASC2AIGameMode* GameMode = Cast<ASC2AIGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->AllMove(Hit.ImpactPoint);
		}
	}
}

void ASC2AIPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASC2AIPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (Distance > 120.0f)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASC2AIPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ASC2AIPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ASC2AIPlayerController::OnMouseClick()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	if (HitResult.GetActor())
	{
		/*AActor* Actor = HitResult.GetActor();
		FString Name = Actor->GetName();*/
		if (ASC2AICharacter* NewSelected = Cast<ASC2AICharacter>(HitResult.GetActor()))
		{
			if (SelectedCharacter)
			{
				if (USkeletalMeshComponent* MeshComp = SelectedCharacter->GetMesh())
				{
					if (DefaultMaterial)
					{
						MeshComp->SetMaterial(0, DefaultMaterial);
					}
				}

				SelectedCharacter->SetCollisionVisible(false);
			}

			NewSelected->SetCollisionVisible(true);

			SelectedCharacter = NewSelected;
		}
		

	}
}

void ASC2AIPlayerController::ShowDebugMessage()
{

}