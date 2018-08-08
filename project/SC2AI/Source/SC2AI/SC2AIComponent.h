// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SC2AIComponent.generated.h"

class ACharacter;
class UBoxComponent;


UENUM(BlueprintType)
enum class EGroup : uint8
{
	Ally,
	Enemy,
};

enum EOverlapBoxIndex
{
	Forward_,	//不加下划线会和引擎代码命名冲突
	Left,
	Right,
	ForwardLeft,
	ForwardRight,
	Max,
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SC2AI_API USC2AIComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USC2AIComponent();

	void InitOverlapBox(ACharacter* Target);

	void SetGroup(EGroup GroupType) { Group = GroupType; }

	EGroup GetGroup() { return Group; }

	void SetDestDirection(const FVector& Direction);

	void GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount);

	void SetCollisionVisible(bool IsVisible);

protected:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called when the game starts
	virtual void BeginPlay() override;

	void CalcMovement(float DeltaSeconds);

	void RefreshLerpData();

	void LerpRotateAndMove(float DeltaSeconds);

	void RotateCollisionForward(float DeltaSeconds);

	void RefreshBlockInfo(float DeltaSeconds);

private:

	//@TODO force inline
	FVector GetMoveDiretion(const FVector& InputVector);

protected:

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

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdLeftBox;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* FwdRightBox;

	FVector DestDirection;

	bool IsCollisionHide;

	TArray<int> CountArray;
	TArray<FVector> DirectionArray;

	float BlockTime;
	float BlockCheckTime;
	float BlockCheckInterval;
	FVector LastPosition;

private:

	//是否鼠标选中(仅调试用)
	bool IsSelected;
};
