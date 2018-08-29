// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "SC2AIGameMode.generated.h"

class ASC2AICharacter;

UCLASS(minimalapi)
class ASC2AIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASC2AIGameMode();

	void AllMove(const FVector& Location);

	void SetAllStopMove();

	void SpawnAgentBegin();

	//bool GetAllStopMove();

protected:

	virtual void Tick(float DeltaSecond) override;

	virtual void StartPlay() override;

protected:

	UFUNCTION()
		void SpawnCharacter();

protected:

	UPROPERTY(BlueprintReadOnly, Category = AI)
		TArray<APawn*> CharList;

private:

	UPROPERTY(EditDefaultsOnly, Category = AI)
	TSubclassOf<APawn> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		FVector DestDirection;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		FRotator SpawnRotOffset;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		FVector SpawnLocOffset;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		FVector LocBase;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		FVector DestLoc;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		float SpawnInterval;

	float SpawnTime;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		int SpawnCountPeerTime;

	bool OffsetFlag;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		float RandDist;

	int CurrSpawnedCount;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		int AllSpawnedCount;

	FTimerDelegate TimerDel;

	bool bSpawnBegin = false;
};



