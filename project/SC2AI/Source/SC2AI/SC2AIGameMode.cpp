// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SC2AIGameMode.h"
#include "SC2AIPlayerController.h"
#include "SC2AICharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Classes/GameFramework/SpectatorPawn.h"
#include "NavigationSystem.h"

#include "SC2AICharacter.h"
#include "MyAIController.h"

ASC2AIGameMode::ASC2AIGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// use our custom PlayerController class
	PlayerControllerClass = ASC2AIPlayerController::StaticClass();

	DestDirection = FVector(0.f, 1.f, 0.f);
	SpawnRotOffset = FRotator(0.f, 90.f, 0.f);
	SpawnLocOffset = FVector(50.f, 0.f, 0.f);
	LocBase = FVector(-70.f, -2000.f, 500.f);
	DestLoc = FVector(70.f, 1540.f, 500.f);
	SpawnInterval = 0.5f;
	SpawnCountPeerTime = 5;
	SpawnTime = 0.f;
	OffsetFlag = false;
	RandDist = 500.f;
	CurrSpawnedCount = 0;
	AllSpawnedCount = 100;

	TimerDel.BindUFunction(this, FName("SpawnCharacter"));
}

void ASC2AIGameMode::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);
}

void ASC2AIGameMode::StartPlay()
{
	Super::StartPlay();

	SpawnCharacter();
}


void ASC2AIGameMode::AllMove(const FVector& Location)
{
	int Count = 0;
	for (ASC2AICharacter* Char : CharList)
	{
		if (Char->IsValidLowLevelFast())
		{
			if (AMyAIController* Controller = Cast<AMyAIController>(Char->GetController()))
			{
				EPathFollowingRequestResult::Type Ret = Controller->MoveToLocation(Location);
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::FromInt((int)Ret));
				Count++;
			}
		}
	}
}

void ASC2AIGameMode::SpawnCharacter()
{
	FVector Loc = LocBase + FVector(FMath::RandRange(-RandDist, RandDist), FMath::RandRange(-RandDist, RandDist), 0.f);
	if (ASC2AICharacter* Char = GetWorld()->SpawnActor<ASC2AICharacter>(CharacterClass, Loc, SpawnRotOffset))
	{
		Char->SpawnDefaultController();
		Char->SetGroup(EGroup::Ally);
		Char->SetDestDirection(DestDirection);
		Char->SetRTSAIEnabled(true);

		CharList.Add(Char);

		CurrSpawnedCount++;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString("QQQQQQQQ"));
	}

	if (CurrSpawnedCount < AllSpawnedCount)
	{
		GetWorldTimerManager().SetTimerForNextTick(TimerDel);
	}
}