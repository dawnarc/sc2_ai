// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RTSCrowdAIComponent.generated.h"

class ACharacter;
class UBoxComponent;
class AController;


UENUM(BlueprintType)
enum class EGroup : uint8
{
	Ally,
	Enemy,
};

namespace RTSAI
{
	enum EOverlapBoxIndex
	{
		Forward,
		Left,
		Right,
		FwdLeft,
		FwdRight,
		Max,
	};
}

struct FakeOverlapBox
{
	FVector LocationOffset;
	FVector Extent;

	FVector WorldLocation;

	int OverlapCount;

	FakeOverlapBox()
	{
		LocationOffset = FVector::ZeroVector;
		Extent = FVector::ZeroVector;
		WorldLocation = FVector::ZeroVector;
		OverlapCount = 0;
	}

	FakeOverlapBox(const FVector& Offset, const FVector& Size)
	{
		LocationOffset = Offset;
		Extent = Size;
		WorldLocation = FVector::ZeroVector;
		OverlapCount = 0;
	}

	bool IsInside(const FVector& TargetLoc, int CapsuleRadius)
	{
		return FMath::Abs(TargetLoc.X - WorldLocation.X) <= Extent.X + CapsuleRadius && FMath::Abs(TargetLoc.Y - WorldLocation.Y) <= Extent.Y + CapsuleRadius && FMath::Abs(TargetLoc.Z - WorldLocation.Z) <= Extent.Z + CapsuleRadius;
	}

	void SetWorldLocation(const FVector& ParentWorldLocation, const FRotator& ParentWorldRotation)
	{
		WorldLocation = ParentWorldLocation + ParentWorldRotation.RotateVector(LocationOffset);
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSAI_API URTSCrowdAIComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URTSCrowdAIComponent();

	void SetEnabled(bool Enable) { IsEnable = Enable; }

	void InitOverlapBox(ACharacter* Target);

	void SetGroup(EGroup GroupType) { Group = GroupType; }

	EGroup GetGroup() { return Group; }

	void SetDestDirection(const FVector& Direction);

	void GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount, int& BigLeftCount, int& BigRightCount);

	void SetCollisionVisible(bool IsVisible);

	void SetCharacterCaptureRadius(float Radius);

protected:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateBox(float DeltaTime);

	void CalcMovement(float DeltaSeconds);

	void RefreshLerpData();

	void LerpRotateAndMove(float DeltaSeconds);

	void RotateCollisionForward(float DeltaSeconds);

	void RefreshBlockInfo(float DeltaSeconds);

private:

	//@TODO force inline
	FVector GetMoveDiretion(const FVector& InputVector);

	//neighbor searching main function.
	void FindNeighborAgents(float DeltaTime);

	FORCEINLINE void EnableMovement();

	FORCEINLINE void DisableMovement();

protected:

	bool IsEnable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EGroup Group;

	float CalcDelayTime;

	const FVector DireFwd = FVector(1.f, 0.f, 0.f);
	const FVector DireLeft = FVector(0.f, -1.f, 0.f);
	const FVector DireRight = FVector(0.f, 1.f, 0.f);
	const FVector DireFwdLeft = FVector(1.f, -1.f, 0.f);
	const FVector DireFwdRight = FVector(1.f, 1.f, 0.f);

	FVector CurrDirection;
	FVector LastDirection;

	float RotateLerpDuration;
	float RotateLerpTime90Degree;
	float RotateLerpTime;

	/*UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdLeftBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdRightBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* LeftBigBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* RightBigBox;*/

	//*********** Overlap check simulate (Begin)**************
	FakeOverlapBox FwdBoxEx = FakeOverlapBox(FVector(75.f, 0.f, 0.f), FVector(20.f, 20.f, 20.f));
	FakeOverlapBox LeftBoxEx = FakeOverlapBox(FVector(0.f, -75.f, 0.f), FVector(20.f, 20.f, 20.f));
	FakeOverlapBox RightBoxEx = FakeOverlapBox(FVector(0.f, 75.f, 0.f), FVector(20.f, 20.f, 20.f));
	FakeOverlapBox FwdLeftBoxEx = FakeOverlapBox(FVector(75.f, -75.f, 0.f), FVector(20.f, 20.f, 20.f));
	FakeOverlapBox FwdRightBoxEx = FakeOverlapBox(FVector(75.f, 75.f, 0.f), FVector(20.f, 20.f, 20.f));
	FakeOverlapBox LeftBigBoxEx = FakeOverlapBox(FVector(60.f, -180.f, 0.f), FVector(100.f, 80.f, 20.f));
	FakeOverlapBox RightBigBoxEx = FakeOverlapBox(FVector(60.f, 180.f, 0.f), FVector(100.f, 80.f, 20.f));

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float CharacterCapsuleRadius;

	//*********** Overlap check simulate (End)**************

	float OverlapCheckTime = 0;
	const float OverlapCheckInterval = 0.25f;

	FVector DestDirection;

	bool IsCollisionHide;

	TArray<int> CountArray;
	TArray<FVector> DirectionArray;

	float BlockTime;
	float BlockCheckTime;
	float BlockCheckInterval;
	FVector LastPosition;

	float NeighborCheckTime = 0.f;
	const float NeighborCheckInterval = 1.f;
	const float NeighborSearchRange = 450.f;
	TArray<APawn*> NeighborAgents;

	UPROPERTY()
	AController* SelfController;

private:

	//is character has been clicked(for debug only)
	bool IsSelected;
};
