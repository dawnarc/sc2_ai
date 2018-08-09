// Fill out your copyright notice in the Description page of Project Settings.

#include "SC2AIComponent.h"
#include "Components/BoxComponent.h"
#include "Math/RotationMatrix.h"

#include "SC2AICharacter.h"

// Sets default values for this component's properties
USC2AIComponent::USC2AIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CalcDelayTime = 1.f;
	RotateLerpTime90Degree = 0.3f;
	RotateLerpDuration = 0.3f;
	RotateLerpTime = 0.f;

	FwdBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FwdBox"));
	FwdBox->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	FwdBox->SetupAttachment(this);
	FwdBox->AddLocalOffset(FVector(75.f, 0.f, 0.f));
	FwdBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	LeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftBox"));
	LeftBox->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	LeftBox->SetupAttachment(this);
	LeftBox->AddLocalOffset(FVector(0.f, -75.f, 0.f));
	LeftBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	RightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightBox"));
	RightBox->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	RightBox->SetupAttachment(this);
	RightBox->AddLocalOffset(FVector(0.f, 75.f, 0.f));
	RightBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	FwdLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FwdLeftBox"));
	FwdLeftBox->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	FwdLeftBox->SetupAttachment(this);
	FwdLeftBox->AddLocalOffset(FVector(75.f, -75.f, 0.f));
	FwdLeftBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	FwdRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FwdRightBox"));
	FwdRightBox->SetBoxExtent(FVector(20.f, 20.f, 20.f));
	FwdRightBox->SetupAttachment(this);
	FwdRightBox->AddLocalOffset(FVector(75.f, 75.f, 0.f));
	FwdRightBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	LeftBigBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftBigBox"));
	LeftBigBox->SetBoxExtent(FVector(100.f, 80.f, 20.f));
	LeftBigBox->SetupAttachment(this);
	LeftBigBox->AddLocalOffset(FVector(60.f, -180.f, 0.f));
	LeftBigBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	RightBigBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightBigBox"));
	RightBigBox->SetBoxExtent(FVector(100.f, 80.f, 20.f));
	RightBigBox->SetupAttachment(this);
	RightBigBox->AddLocalOffset(FVector(60.f, 180.f, 0.f));
	RightBigBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));

	DestDirection = FVector::ZeroVector;

	IsCollisionHide = true;

	BlockTime = 0.f;
	BlockCheckTime = 0.f;
	BlockCheckInterval = 0.3f;
	LastPosition = FVector::ZeroVector;

	IsSelected = false;
}

// Called every frame
void USC2AIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsSelected)
	{
		int temp = 0;
	}

	if (EGroup::Enemy == Group)
	{
		return;
	}

	if (FVector::ZeroVector == DestDirection)
	{
		return;
	}

	CalcMovement(DeltaTime);

	LerpRotateAndMove(DeltaTime);

	RotateCollisionForward(DeltaTime);

	RefreshBlockInfo(DeltaTime);
}

// Called when the game starts
void USC2AIComponent::BeginPlay()
{
	Super::BeginPlay();

	DirectionArray.Add(DireFwd);
	DirectionArray.Add(DireLeft);
	DirectionArray.Add(DireRight);
	DirectionArray.Add(DireFwdLeft);
	DirectionArray.Add(DireFwdRight);
}

void USC2AIComponent::CalcMovement(float DeltaSeconds)
{
	if (CalcDelayTime > 0.f)
	{
		CalcDelayTime -= DeltaSeconds;
		return;
	}

	if (RotateLerpTime < RotateLerpDuration)
	{
		return;
	}
	else
	{
		LastDirection = CurrDirection;

		if (FwdBox)
		{
			TArray<AActor*> OverlapActors;

			FwdBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
			int FwdCount = OverlapActors.Num();

			LeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
			int LeftCount = OverlapActors.Num();

			RightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
			int RightCount = OverlapActors.Num();

			FwdLeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
			int FwdLeftCount = OverlapActors.Num();

			FwdRightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
			int FwdRightCount = OverlapActors.Num();

			//if (LeftCount > 0 && RightCount > 0 && FwdLeftCount > 0 && FwdRightCount > 0)
			if((FwdCount < 3 && LeftCount > 0 && RightCount > 0) && 0.f == BlockTime)
			{
				CurrDirection = GetMoveDiretion(DireFwd);
			}
			else
			{
				CountArray.Reset();

				if (FwdBox)
				{
					FwdBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (LeftBox)
				{
					LeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (RightBox)
				{
					RightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (FwdLeftBox)
				{
					FwdLeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (FwdRightBox)
				{
					FwdRightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (IsSelected)
				{
					int temp = 0;
				}

				if (CountArray.Num() == DirectionArray.Num() && DirectionArray.Num() == EOverlapBoxIndex::Max)
				{
					int32 LessestCount = 999999;
					int Index = -1;
					for (int i = 0; i < CountArray.Num(); i++)
					{
						if (CountArray[i] >= 0)
						{
							if (CountArray[i] < LessestCount)
							{
								Index = i;
								LessestCount = CountArray[i];
								//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, CurrDirection.ToString());
							}
						}
					}

					//find the direction that units count is least.
					if (EOverlapBoxIndex::Left == Index)
					{
						if (CountArray[EOverlapBoxIndex::Left] == CountArray[EOverlapBoxIndex::Right])
						{
							if (CountArray[EOverlapBoxIndex::FwdLeft] == CountArray[EOverlapBoxIndex::FwdRight])
							{
								Index = FMath::RandBool() ? EOverlapBoxIndex::Left : EOverlapBoxIndex::Right;
							}
							else
							{
								Index = CountArray[EOverlapBoxIndex::FwdLeft] > CountArray[EOverlapBoxIndex::FwdRight] ? EOverlapBoxIndex::Right : EOverlapBoxIndex::Left;
							}
						}
					}
					else if(EOverlapBoxIndex::FwdLeft == Index)
					{
						if (CountArray[EOverlapBoxIndex::FwdLeft] == CountArray[EOverlapBoxIndex::FwdRight])
						{
							if (CountArray[EOverlapBoxIndex::Left] == CountArray[EOverlapBoxIndex::Right])
							{
								Index = FMath::RandBool() ? EOverlapBoxIndex::FwdLeft : EOverlapBoxIndex::FwdRight;
							}
							else
							{
								Index = CountArray[EOverlapBoxIndex::Left] > CountArray[EOverlapBoxIndex::Right] ? EOverlapBoxIndex::FwdRight : EOverlapBoxIndex::FwdLeft;
							}
						}
					}

					//if the difference of overlap count in two side BigBox is greater than 3, thus we choose the direction of BigBox that overlap count is least.
					LeftBigBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					int LeftBigCount = OverlapActors.Num();

					RightBigBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
					int RightBigCount = OverlapActors.Num();

					if (FMath::Abs(LeftBigCount - RightBigCount) >= 4)
					{
						if (FMath::Abs(LeftCount - RightCount) <= 1 || FMath::Abs(FwdLeftCount - FwdRightCount) <= 1 /*&& (EOverlapBoxIndex::Left == Index || EOverlapBoxIndex::Right == Index)*/)
						{
							Index = LeftBigCount > RightBigCount ? EOverlapBoxIndex::Right : EOverlapBoxIndex::Left;
						}
						/*else if (FMath::Abs(FwdLeftCount - FwdRightCount) == 1 && (EOverlapBoxIndex::FwdLeft == Index || EOverlapBoxIndex::FwdRight == Index))
						{
							Index = LeftBigCount > RightBigCount ? EOverlapBoxIndex::FwdRight : EOverlapBoxIndex::FwdLeft;
						}*/
					}

					CurrDirection = GetMoveDiretion(DirectionArray[Index]);
				}
			}
		}

		RefreshLerpData();
	}
}

void USC2AIComponent::RefreshLerpData()
{
	if (!LastDirection.Equals(CurrDirection))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Emerald, FString("TTTTTTTTTTTT"));
		RotateLerpTime = 0.f;
		const float DotPro = FVector::DotProduct(LastDirection, CurrDirection);
		RotateLerpDuration = DotPro == -1.f ? RotateLerpTime90Degree * 2 : RotateLerpTime90Degree;
	}
}

void USC2AIComponent::LerpRotateAndMove(float DeltaSeconds)
{
	if (ASC2AICharacter* Character = Cast<ASC2AICharacter>(GetAttachmentRootActor()))
	{
		if (RotateLerpTime < RotateLerpDuration)
		{
			RotateLerpTime += DeltaSeconds;

			FVector LerpA = LastDirection;
			FVector LerpB = CurrDirection;
			float Alpha = 0.f;

			//if angle between LastDirection and CurrDirection equal 180, turn to DestDire first, then turn to CurrDirection, avoid to turn backward.
			const float DotPro = FVector::DotProduct(LastDirection, CurrDirection);
			if (DotPro == -1.f)
			{
				float CharDotPro = FVector::DotProduct(Character->GetActorRotation().Vector(), CurrDirection);
				if (CharDotPro < 0.f)
				{
					//from 180 degree to 90 degree.
					LerpB = DestDirection;
					Alpha = RotateLerpTime / (RotateLerpDuration / 2);
				}
				else
				{
					//from 90 degree to 0 degree.
					LerpA = DestDirection;
					Alpha = (RotateLerpTime - (RotateLerpDuration / 2)) / (RotateLerpDuration / 2);
				}
			}
			else
			{
				Alpha = RotateLerpTime / RotateLerpDuration;
			}

			FVector NewDire = FMath::Lerp(LerpA, LerpB, Alpha);
			Character->AddMovementInput(NewDire);
		}
		else
		{
			Character->AddMovementInput(CurrDirection);
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, CurrDirection.ToString());
		}
	}
}

void USC2AIComponent::RotateCollisionForward(float DeltaSeconds)
{
	SetWorldRotation(DestDirection.Rotation());
}

void USC2AIComponent::RefreshBlockInfo(float DeltaSeconds)
{
	if (IsSelected)
	{
		int temp = 0;
	}

	if (BlockCheckTime < BlockCheckInterval)
	{
		BlockCheckTime += DeltaSeconds;
	}
	else
	{
		BlockCheckTime = 0.f;

		if (FVector::Dist(GetComponentLocation(), LastPosition) < 30.f)
		{
			BlockTime += DeltaSeconds;
		}
		else
		{
			BlockTime = 0.f;
		}
		LastPosition = GetComponentLocation();
	}
}

FVector USC2AIComponent::GetMoveDiretion(const FVector& InputVector)
{
	FVector Ret = FVector::ZeroVector;
	if (ASC2AICharacter* Character = Cast<ASC2AICharacter>(GetAttachmentRootActor()))
	{
		//FRotator Rot = (FRotationMatrix(InputVector.Rotation()) * FRotationMatrix(Character->GetActorRotation())).Rotator();
		FRotator Rot = (FRotationMatrix(InputVector.Rotation()) * FRotationMatrix(DestDirection.Rotation())).Rotator();
		Ret = Rot.Vector();
		//Ret = Character->GetActorRotation().Vector();
	}

	return Ret.GetSafeNormal();
}

void USC2AIComponent::InitOverlapBox(ACharacter* Target)
{
	
}

void USC2AIComponent::SetDestDirection(const FVector& Direction)
{
	DestDirection = Direction;
	CurrDirection = GetMoveDiretion(DireFwd);
}

void USC2AIComponent::GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount, int& BigLeftCount, int& BigRightCount)
{
	TArray<AActor*> OverlapActors;

	FwdBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	FwdCount = OverlapActors.Num();

	LeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	LeftCount = OverlapActors.Num();

	RightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	RightCount = OverlapActors.Num();

	FwdLeftBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	FwdLeftCount = OverlapActors.Num();

	FwdRightBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	FwdRightCount = OverlapActors.Num();

	LeftBigBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	BigLeftCount = OverlapActors.Num();

	RightBigBox->GetOverlappingActors(OverlapActors, ASC2AICharacter::StaticClass());
	BigRightCount = OverlapActors.Num();
}

void USC2AIComponent::SetCollisionVisible(bool IsVisible)
{
	FwdBox->SetHiddenInGame(!IsVisible);
	LeftBox->SetHiddenInGame(!IsVisible);
	RightBox->SetHiddenInGame(!IsVisible);
	FwdLeftBox->SetHiddenInGame(!IsVisible);
	FwdRightBox->SetHiddenInGame(!IsVisible);
	LeftBigBox->SetHiddenInGame(!IsVisible);
	RightBigBox->SetHiddenInGame(!IsVisible);

	IsSelected = IsVisible;
}