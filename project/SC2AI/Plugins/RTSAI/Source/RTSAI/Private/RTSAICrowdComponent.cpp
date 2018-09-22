// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSAICrowdComponent.h"
#include "Components/BoxComponent.h"
#include "Math/RotationMatrix.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include "RTSAIContainer.h"


// Sets default values for this component's properties
URTSAICrowdComponent::URTSAICrowdComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
}

// Called every frame
void URTSAICrowdComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsEnable)
	{
		return;
	}

	if (IsSelected)
	{
		//for breakpoint (debug only)
		int temp = 0;
	}

	if (FVector::ZeroVector == DestDirection)
	{
		return;
	}

	FindNeighborAgents(DeltaTime);

	UpdateBox(DeltaTime);

	CalcMovement(DeltaTime);

	LerpRotateAndMove(DeltaTime);

	RotateCollisionForward(DeltaTime);

	RefreshBlockState(DeltaTime);
}

// Called when the game starts
void URTSAICrowdComponent::BeginPlay()
{
	Super::BeginPlay();

	DirectionArray.Add(DireFwd);
	DirectionArray.Add(DireLeft);
	DirectionArray.Add(DireRight);
	DirectionArray.Add(DireFwdLeft);
	DirectionArray.Add(DireFwdRight);
}

void URTSAICrowdComponent::UpdateBox(float DeltaTime)
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

		if (APawn* Pawn = Cast<APawn>(GetAttachmentRootActor()))
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

			//for (TActorIterator<APawn> Iter(GetWorld()); Iter; ++Iter)
			for(APawn* Agent : NeighborAgents)
			{
				if (Agent != Pawn && Agent->IsValidLowLevelFast())
				{
					FVector AgentLoc = Agent->GetActorLocation();

					if (FwdBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						FwdBoxEx.OverlapCount++;
					}

					if (LeftBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						LeftBoxEx.OverlapCount++;
					}

					if (RightBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						RightBoxEx.OverlapCount++;
					}

					if (FwdLeftBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						FwdLeftBoxEx.OverlapCount++;
					}

					if (FwdRightBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						FwdRightBoxEx.OverlapCount++;
					}

					if (LeftBigBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						LeftBigBoxEx.OverlapCount++;
					}

					if (RightBigBoxEx.IsInside(AgentLoc, CharacterCapsuleRadius))
					{
						RightBigBoxEx.OverlapCount++;
					}
				}
			}
		}
	}
}

void URTSAICrowdComponent::CalcMovement(float DeltaSeconds)
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
			if((FwdCount < 3 && LeftCount > 0 && RightCount > 0) && !bIsBlocked || (LeftBigCount < 7 && RightBigCount < 7))
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

				if (CountArray.Num() == DirectionArray.Num() && DirectionArray.Num() == RTSAI::Max)
				{
					//find the direction that units count is least.
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

					//random direction if neighbor count of left and right are equivalent.
					if (RTSAI::Left == Index)
					{
						if (CountArray[RTSAI::Left] == CountArray[RTSAI::Right])
						{
							if (CountArray[RTSAI::FwdLeft] == CountArray[RTSAI::FwdRight])
							{
								Index = FMath::RandBool() ? RTSAI::Left : RTSAI::Right;
							}
							else
							{
								Index = CountArray[RTSAI::FwdLeft] > CountArray[RTSAI::FwdRight] ? RTSAI::Right : RTSAI::Left;
							}
						}
					}
					else if (RTSAI::FwdLeft == Index)
					{
						if (CountArray[RTSAI::FwdLeft] == CountArray[RTSAI::FwdRight])
						{
							if (CountArray[RTSAI::Left] == CountArray[RTSAI::Right])
							{
								Index = FMath::RandBool() ? RTSAI::FwdLeft : RTSAI::FwdRight;
							}
							else
							{
								Index = CountArray[RTSAI::Left] > CountArray[RTSAI::Right] ? RTSAI::FwdRight : RTSAI::FwdLeft;
							}
						}
					}

					//if the difference of overlap count in two side BigBox is greater than 3, thus we choose the direction of BigBox that overlap count is least.
					if (FMath::Abs(LeftBigCount - RightBigCount) >= 4)
					{
						if (FMath::Abs(LeftCount - RightCount) <= 1 || FMath::Abs(FwdLeftCount - FwdRightCount) <= 1)
						{
							Index = LeftBigCount > RightBigCount ? RTSAI::Right : RTSAI::Left;
						}
					}

					CurrDirection = GetMoveDiretion(DirectionArray[Index]);
				}
			}
		}

		RefreshLerpData();
	}
}

void URTSAICrowdComponent::RefreshLerpData()
{
	if (!LastDirection.Equals(CurrDirection))
	{
		RotateLerpTime = 0.f;
		const float DotPro = FVector::DotProduct(LastDirection, CurrDirection);
		RotateLerpDuration = DotPro == -1.f ? RotateLerpTime90Degree * 2 : RotateLerpTime90Degree;
	}
}

void URTSAICrowdComponent::LerpRotateAndMove(float DeltaSeconds)
{
	if (IsSelected)
	{
		int temp = 0;
	}

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

void URTSAICrowdComponent::RotateCollisionForward(float DeltaSeconds)
{
	SetWorldRotation(DestDirection.Rotation());
}

void URTSAICrowdComponent::RefreshBlockState(float DeltaSeconds)
{
	if (IsSelected)
	{
		int temp = 0;
	}

	static const float MIN_MOVE_DIST = 2.5f;
	if (bIsBlocked)
	{
		if (BlockCheckDuration <= BlockCheckInterval)
		{
			BlockCheckDuration += DeltaSeconds;
		}
		else
		{
			BlockCheckDuration = 0.f;
			bIsBlocked = false;
		}
	}
	else
	{
		if (bBlockChecked)
		{
			if (UnblockCheckDuration <= UnblockCheckInterval)
			{
				UnblockCheckDuration += DeltaSeconds;
			}
			else
			{
				UnblockCheckDuration = 0.f;
				bBlockChecked = false;
			}
		}
		else
		{
			StopCheckCount++;

			if (StopCheckCount > 5)
			{
				bBlockChecked = true;
				StopCount = 0;
				StopCheckCount = 0;
			}
			
			float Dist = FVector::Dist(GetComponentLocation(), LastPosition);
			DebugDist = Dist;

			if (Dist < MIN_MOVE_DIST)
			{
				StopCount++;
			}
			else
			{
				StopCount = 0.f;
			}

			if (StopCount > STOP_FRAMES_COUNT_THRESHOLD)
			{
				bIsBlocked = true;
				DisableMovement();
			}
			else
			{
				bIsBlocked = false;
				EnableMovement();
			}
		}
	}

	LastPosition = GetComponentLocation();
}

FVector URTSAICrowdComponent::GetMoveDiretion(const FVector& InputVector)
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

void URTSAICrowdComponent::FindNeighborAgents(float DeltaTime)
{
	if (IsSelected)
	{
		int Temp = 0;
	}

	if (NeighborCheckTime < NeighborCheckInterval)
	{
		NeighborCheckTime += DeltaTime;
	}
	else
	{
		NeighborCheckTime = 0.f;

		if (const APawn* Pawn = Cast<APawn>(GetAttachmentRootActor()))
		{
			FVector SelfLoc = Pawn->GetActorLocation();

			NeighborAgents.Reset();

			for (TActorIterator<APawn> Iter(GetWorld()); Iter; ++Iter)
			{
				if (Pawn != *Iter)
				{
					FVector AgentLoc = Iter->GetActorLocation();

					if (FVector::Dist(SelfLoc, AgentLoc) < NeighborSearchRange)
					{
						NeighborAgents.Add(*Iter);
					}
				}
			}
		}
	}
}

void URTSAICrowdComponent::EnableMovement()
{
	if (APawn* Pawn = Cast<APawn>(GetAttachmentRootActor()))
	{
		if (!Pawn->GetController() && SelfController)
		{
			Pawn->PossessedBy(SelfController);
		}
	}
}

void URTSAICrowdComponent::DisableMovement()
{
	if (APawn* Pawn = Cast<APawn>(GetAttachmentRootActor()))
	{
		if (!SelfController)
		{
			SelfController = Pawn->GetController();
		}

		if (Pawn->GetController())
		{
			Pawn->UnPossessed();
		}
	}
}

void URTSAICrowdComponent::InitOverlapBox(ACharacter* Target)
{
	
}

void URTSAICrowdComponent::SetDestDirection(const FVector& Direction)
{
	DestDirection = Direction;
	CurrDirection = GetMoveDiretion(DireFwd);
}

void URTSAICrowdComponent::GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount, int& BigLeftCount, int& BigRightCount)
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

void URTSAICrowdComponent::SetCollisionVisible(bool IsVisible)
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

void URTSAICrowdComponent::SetCharacterCaptureRadius(float Radius)
{
	CharacterCapsuleRadius = Radius;
}