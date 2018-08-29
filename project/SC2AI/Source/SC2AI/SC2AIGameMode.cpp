// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SC2AIGameMode.h"
#include "SC2AIPlayerController.h"
#include "SC2AICharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Classes/GameFramework/SpectatorPawn.h"
#include "NavigationSystem.h"

#include "SC2AICharacter.h"
#include "MyAIController.h"
#include "MyPawn.h"

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
	SpawnInterval = 0.8f;
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
}


void ASC2AIGameMode::AllMove(const FVector& Location)
{
	int Count = 0;
	/*for (ASC2AICharacter* Char : CharList)
	{
		if (Char->IsValidLowLevelFast())
		{
			if (AMyAIController* Controller = Cast<AMyAIController>(Char->GetController()))
			{
				EPathFollowingRequestResult::Type Ret = Controller->MoveToLocation(Location);
				Count++;
			}
		}
	}*/
}

void ASC2AIGameMode::SetAllStopMove()
{
	for (APawn* Pawn : CharList)
	{
		if (ASC2AICharacter* Char = Cast<ASC2AICharacter>(Pawn))
		{
			Char->EnableMove(false);
		}
	}
}

void ASC2AIGameMode::SpawnAgentBegin()
{
	SpawnCharacter();
}

//bool ASC2AIGameMode::GetAllStopMove()
//{
//
//}

void ASC2AIGameMode::SpawnCharacter()
{
	if (GetWorld())
	{
		SpawnTime += GetWorld()->GetDeltaSeconds();

		if (SpawnTime >= SpawnInterval)
		{
			SpawnTime = 0.f;

			for (int i = 0; i < SpawnCountPeerTime; i++)
			{
				if (CurrSpawnedCount >= AllSpawnedCount)
				{
					break;
				}

				FVector Loc = LocBase + FVector(FMath::RandRange(-RandDist, RandDist), FMath::RandRange(-RandDist, RandDist), 0.f);
				

				//Loc.Z = 2000.f;

				
				if (APawn* Char = GetWorld()->SpawnActor<APawn>(CharacterClass, Loc, SpawnRotOffset))
				{
					if (ASC2AICharacter* SC2Char = Cast<ASC2AICharacter>(Char))
					{
						SC2Char->SetGroup(EGroup::Ally);
						SC2Char->SpawnDefaultController();
						SC2Char->InitRTSAIComponent();
						SC2Char->SetRTSAIDestDirection(DestDirection);
						SC2Char->SetRTSAIEnabled(true);

						/*Char->EnableMove(true);
						Char->MoveDirection = DestDirection;*/
					}

					CharList.Add(Char);

					CurrSpawnedCount++;
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Magenta, FString("QQQQQQQQQQQQQQQQQQQQQ"));
				}
			}
		}

		if (CurrSpawnedCount < AllSpawnedCount)
		{
			GetWorldTimerManager().SetTimerForNextTick(TimerDel);
		}
	}
}