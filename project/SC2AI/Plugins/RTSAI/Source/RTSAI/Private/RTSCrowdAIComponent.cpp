// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSCrowdAIComponent.h"
#include "Components/BoxComponent.h"
#include "Math/RotationMatrix.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
URTSCrowdAIComponent::URTSCrowdAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CalcDelayTime = 1.f;
	RotateLerpTime90Degree = 0.3f;
	RotateLerpDuration = 0.3f;
	RotateLerpTime = 0.f;

	/*FwdBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FwdBox"));
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
	RightBigBox->SetCollisionProfileName(TEXT("RoundBoxPreset"));*/

	CharacterCapsuleRadius = 42.f;

	DestDirection = FVector::ZeroVector;

	IsCollisionHide = true;

	BlockTime = 0.f;
	BlockCheckTime = 0.f;
	BlockCheckInterval = 0.3f;
	LastPosition = FVector::ZeroVector;

	IsSelected = false;
}

// Called every frame
void URTSCrowdAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

	UpdateBox(DeltaTime);

	CalcMovement(DeltaTime);

	LerpRotateAndMove(DeltaTime);

	RotateCollisionForward(DeltaTime);

	RefreshBlockInfo(DeltaTime);
}

// Called when the game starts
void URTSCrowdAIComponent::BeginPlay()
{
	Super::BeginPlay();

	DirectionArray.Add(DireFwd);
	DirectionArray.Add(DireLeft);
	DirectionArray.Add(DireRight);
	DirectionArray.Add(DireFwdLeft);
	DirectionArray.Add(DireFwdRight);
}

void URTSCrowdAIComponent::UpdateBox(float DeltaTime)
{
	if (OverlapCheckTime < OverlapCheckInterval)
	{
		OverlapCheckTime += DeltaTime;
	}
	else
	{
		OverlapCheckTime = 0.f;

		const FVector CompLoc = GetComponentLocation();
		const FRotator CompRot = GetComponentRotation();

		FwdBoxEx.SetWorldLocation(CompLoc, CompRot);
		LeftBoxEx.SetWorldLocation(CompLoc, CompRot);
		RightBoxEx.SetWorldLocation(CompLoc, CompRot);
		FwdLeftBoxEx.SetWorldLocation(CompLoc, CompRot);
		FwdRightBoxEx.SetWorldLocation(CompLoc, CompRot);
		LeftBigBoxEx.SetWorldLocation(CompLoc, CompRot);
		RightBigBoxEx.SetWorldLocation(CompLoc, CompRot);

		if (APawn* Parent = Cast<APawn>(GetAttachmentRootActor()))
		{
			FwdBoxEx.OverlapCount = 0.f;
			LeftBoxEx.OverlapCount = 0.f;
			RightBoxEx.OverlapCount = 0.f;
			FwdLeftBoxEx.OverlapCount = 0.f;
			FwdRightBoxEx.OverlapCount = 0.f;
			LeftBigBoxEx.OverlapCount = 0.f;
			RightBigBoxEx.OverlapCount = 0.f;

			if (IsSelected)
			{
				//for breakpoint
				int temp = 0;
			}

			for (TActorIterator<APawn> Iter(GetWorld()); Iter; ++Iter)
			{
				if (*Iter != Parent)
				{
					FVector UnitLoc = Iter->GetActorLocation();

					if (FwdBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						FwdBoxEx.OverlapCount++;
					}

					if (LeftBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						LeftBoxEx.OverlapCount++;
					}

					if (RightBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						RightBoxEx.OverlapCount++;
					}

					if (FwdLeftBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						FwdLeftBoxEx.OverlapCount++;
					}

					if (FwdRightBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						FwdRightBoxEx.OverlapCount++;
					}

					if (LeftBigBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						LeftBigBoxEx.OverlapCount++;
					}

					if (RightBigBoxEx.IsInside(UnitLoc, CharacterCapsuleRadius))
					{
						RightBigBoxEx.OverlapCount++;
					}
				}
			}
		}
	}
}

void URTSCrowdAIComponent::CalcMovement(float DeltaSeconds)
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

		if (1/*FwdBox && LeftBox && RightBox && FwdLeftBox && FwdRightBox && LeftBigBox && RightBigBox*/)
		{
			/*TArray<AActor*> OverlapActors;

			FwdBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int FwdCount = OverlapActors.Num();

			LeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int LeftCount = OverlapActors.Num();

			RightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int RightCount = OverlapActors.Num();

			FwdLeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int FwdLeftCount = OverlapActors.Num();

			FwdRightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int FwdRightCount = OverlapActors.Num();

			LeftBigBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int LeftBigCount = OverlapActors.Num();

			RightBigBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
			int RightBigCount = OverlapActors.Num();*/

			int FwdCount = FwdBoxEx.OverlapCount;
			int LeftCount = LeftBoxEx.OverlapCount;
			int RightCount = RightBoxEx.OverlapCount;
			int FwdLeftCount = FwdLeftBoxEx.OverlapCount;
			int FwdRightCount = FwdRightBoxEx.OverlapCount;
			int LeftBigCount = LeftBigBoxEx.OverlapCount;
			int RightBigCount = RightBigBoxEx.OverlapCount;

			if (IsSelected)
			{
				//for breakpoint
				int temp = 0;
			}

			//if (LeftCount > 0 && RightCount > 0 && FwdLeftCount > 0 && FwdRightCount > 0)
			if((FwdCount < 3 && LeftCount > 0 && RightCount > 0) && 0.f == BlockTime || (LeftBigCount < 7 && RightBigCount < 7))
			{
				CurrDirection = GetMoveDiretion(DireFwd);
			}
			else
			{
				CountArray.Reset();

				CountArray.Add(FwdCount);
				CountArray.Add(LeftCount);
				CountArray.Add(RightCount);
				CountArray.Add(FwdLeftCount);
				CountArray.Add(FwdRightCount);

				/*if (FwdBox)
				{
					FwdBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (LeftBox)
				{
					LeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (RightBox)
				{
					RightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (FwdLeftBox)
				{
					FwdLeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}

				if (FwdRightBox)
				{
					FwdRightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
					CountArray.Add(OverlapActors.Num());
				}*/

				if (IsSelected)
				{
					//for breakpoint
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
					if (FMath::Abs(LeftBigCount - RightBigCount) >= 4)
					{
						if (FMath::Abs(LeftCount - RightCount) <= 1 || FMath::Abs(FwdLeftCount - FwdRightCount) <= 1)
						{
							Index = LeftBigCount > RightBigCount ? EOverlapBoxIndex::Right : EOverlapBoxIndex::Left;
						}
					}

					CurrDirection = GetMoveDiretion(DirectionArray[Index]);
				}
			}
		}

		RefreshLerpData();
	}
}

void URTSCrowdAIComponent::RefreshLerpData()
{
	if (!LastDirection.Equals(CurrDirection))
	{
		RotateLerpTime = 0.f;
		const float DotPro = FVector::DotProduct(LastDirection, CurrDirection);
		RotateLerpDuration = DotPro == -1.f ? RotateLerpTime90Degree * 2 : RotateLerpTime90Degree;
	}
}

void URTSCrowdAIComponent::LerpRotateAndMove(float DeltaSeconds)
{
	if (APawn* Pawn = Cast<APawn>(GetAttachmentRootActor()))
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
				float CharDotPro = FVector::DotProduct(Pawn->GetActorRotation().Vector(), CurrDirection);
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
			Pawn->AddMovementInput(NewDire);
		}
		else
		{
			Pawn->AddMovementInput(CurrDirection);
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, CurrDirection.ToString());
		}
	}
}

void URTSCrowdAIComponent::RotateCollisionForward(float DeltaSeconds)
{
	SetWorldRotation(DestDirection.Rotation());
}

void URTSCrowdAIComponent::RefreshBlockInfo(float DeltaSeconds)
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

FVector URTSCrowdAIComponent::GetMoveDiretion(const FVector& InputVector)
{
	FVector Ret = FVector::ZeroVector;
	if (APawn* Character = Cast<APawn>(GetAttachmentRootActor()))
	{
		//FRotator Rot = (FRotationMatrix(InputVector.Rotation()) * FRotationMatrix(Character->GetActorRotation())).Rotator();
		FRotator Rot = (FRotationMatrix(InputVector.Rotation()) * FRotationMatrix(DestDirection.Rotation())).Rotator();
		Ret = Rot.Vector();
		//Ret = Character->GetActorRotation().Vector();
	}

	return Ret.GetSafeNormal();
}

void URTSCrowdAIComponent::InitOverlapBox(ACharacter* Target)
{
	
}

void URTSCrowdAIComponent::SetDestDirection(const FVector& Direction)
{
	DestDirection = Direction;
	CurrDirection = GetMoveDiretion(DireFwd);
}

void URTSCrowdAIComponent::GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount, int& BigLeftCount, int& BigRightCount)
{
	/*TArray<AActor*> OverlapActors;

	FwdBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	FwdCount = OverlapActors.Num();

	LeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	LeftCount = OverlapActors.Num();

	RightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	RightCount = OverlapActors.Num();

	FwdLeftBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	FwdLeftCount = OverlapActors.Num();

	FwdRightBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	FwdRightCount = OverlapActors.Num();

	LeftBigBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	BigLeftCount = OverlapActors.Num();

	RightBigBox->GetOverlappingActors(OverlapActors, APawn::StaticClass());
	BigRightCount = OverlapActors.Num();*/

	FwdCount = FwdBoxEx.OverlapCount;
	LeftCount = LeftBoxEx.OverlapCount;
	RightCount = RightBoxEx.OverlapCount;
	FwdLeftCount = FwdLeftBoxEx.OverlapCount;
	FwdRightCount = FwdRightBoxEx.OverlapCount;
	BigLeftCount = LeftBigBoxEx.OverlapCount;
	BigRightCount = RightBigBoxEx.OverlapCount;
}

void URTSCrowdAIComponent::SetCollisionVisible(bool IsVisible)
{
	/*if (FwdBox)
	{
		FwdBox->SetHiddenInGame(!IsVisible);
	}

	if (LeftBox)
	{
		LeftBox->SetHiddenInGame(!IsVisible);
	}

	if (RightBox)
	{
		RightBox->SetHiddenInGame(!IsVisible);
	}

	if (FwdLeftBox)
	{
		FwdLeftBox->SetHiddenInGame(!IsVisible);
	}

	if (FwdRightBox)
	{
		FwdRightBox->SetHiddenInGame(!IsVisible);
	}

	if (LeftBigBox)
	{
		LeftBigBox->SetHiddenInGame(!IsVisible);
	}

	if (RightBigBox)
	{
		RightBigBox->SetHiddenInGame(!IsVisible);
	}*/

	IsSelected = IsVisible;
}

void URTSCrowdAIComponent::SetCharacterCaptureRadius(float Radius)
{
	CharacterCapsuleRadius = Radius;
}