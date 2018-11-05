// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSAIBlockActor.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DrawDebugHelpers.h"

#include "RTSAIUtil.h"
#include "RTSAICrowdComponent.h"

// Sets default values
ARTSAIBlockActor::ARTSAIBlockActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DelayCheckTime = FMath::RandRange(0.1f, 0.5f);

	CustomRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("CustomRootComp"));
	SetRootComponent(CustomRootComp);

	/*ClimbLineHorizonComp = CreateDefaultSubobject<USplineComponent>(TEXT("ClimbLineHorizonComp"));
	ClimbLineHorizonComp->SetupAttachment(ClimbLineVerticalComp);*/

	EdgeLeftFrontComp = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftEdgeComp"));
	EdgeLeftFrontComp->SetupAttachment(CustomRootComp);
	EdgeLeftFrontComp->SetRelativeLocation(TempEdgeLeftFrontLoc);
	EdgeLeftFrontComp->SetRelativeRotation(TempEdgeLeftFrontRot);
	EdgeLeftFrontComp->SetArrowColor(ArrowColor);

	EdgeLeftBackComp = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftEdgePassComp"));
	EdgeLeftBackComp->SetupAttachment(CustomRootComp);
	EdgeLeftBackComp->SetRelativeLocation(TempEdgeLeftBackLoc);
	EdgeLeftBackComp->SetRelativeRotation(TempEdgeLeftBackRot);
	EdgeLeftBackComp->SetArrowColor(ArrowColor);

	EdgeRightFrontComp = CreateDefaultSubobject<UArrowComponent>(TEXT("RightEdgeComp"));
	EdgeRightFrontComp->SetupAttachment(CustomRootComp);
	EdgeRightFrontComp->SetRelativeLocation(TempEdgeRightFrontLoc);
	EdgeRightFrontComp->SetRelativeRotation(TempEdgeRightFrontRot);
	EdgeRightFrontComp->SetArrowColor(ArrowColor);

	EdgeRightBackComp = CreateDefaultSubobject<UArrowComponent>(TEXT("RightEdgePassComp"));
	EdgeRightBackComp->SetupAttachment(CustomRootComp);
	EdgeRightBackComp->SetRelativeLocation(TempEdgeRightBackLoc);
	EdgeRightBackComp->SetRelativeRotation(TempEdgeRightBackRot);
	EdgeRightBackComp->SetArrowColor(ArrowColor);
}


void ARTSAIBlockActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*DrawDebugSphere(GetWorld(), DebugPoint1, 10.f, 50, FColor::Red);
	DrawDebugSphere(GetWorld(), DebugPoint2, 10.f, 50, FColor::Blue);
	DrawDebugSphere(GetWorld(), DebugPoint3, 10.f, 50, FColor::Orange);
	DrawDebugSphere(GetWorld(), DebugPoint4, 10.f, 50, FColor::Purple);*/

	if (!bEnabled)
	{
		return;
	}

	if (DelayCheckTime > 0.f)
	{
		DelayCheckTime -= DeltaSeconds;
	}
	else
	{
		FindAroundPawns(DeltaSeconds);

		EnterClimbAreaCheck(DeltaSeconds);
	}
}

void ARTSAIBlockActor::BeginPlay()
{
	Super::BeginPlay();

	if (RootComponent)
	{
		RootLocation = RootComponent->GetComponentLocation();
	}

	//计算箭头方向向量
	if (EdgeLeftFrontComp)
	{
		EdgeLeftLocation = EdgeLeftFrontComp->GetComponentLocation();
		EdgeLeftFrontDirection = EdgeLeftFrontComp->GetComponentRotation().Vector().GetSafeNormal();
	}
	if (EdgeLeftBackComp)
	{
		EdgeLeftBackDirection = EdgeLeftBackComp->GetComponentRotation().Vector().GetSafeNormal();
	}
	if (EdgeRightFrontComp)
	{
		EdgeRightLocation = EdgeRightFrontComp->GetComponentLocation();
		EdgeRightFrontDirection = EdgeRightFrontComp->GetComponentRotation().Vector().GetSafeNormal();
	}
	if (EdgeRightBackComp)
	{
		EdgeRightBackDirection = EdgeRightBackComp->GetComponentRotation().Vector().GetSafeNormal();
	}

	TurnDirecionPanelNormal = (EdgeRightLocation - EdgeLeftLocation).GetSafeNormal();
	TurnDirecionMidPosition = (EdgeRightLocation + EdgeLeftLocation) / 2;
	DebugPoint3 = TurnDirecionMidPosition;
}

#if WITH_EDITOR
void ARTSAIBlockActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SetCustomProperties();
}
#endif

void ARTSAIBlockActor::PostLoad()
{
	Super::PostLoad();

	//如果不在PostLoad中设置一次箭头的RelativeLocation，则ArrowComp在场景编辑器中的显示有问题，使用的还是构造函数中的相对坐标
	SetCustomProperties();
}

void ARTSAIBlockActor::SetArrowRelativeLocatin(UArrowComponent* ArrowComp, const FVector& TemplateLoc, const FLinearColor& Color)
{
	if (ArrowComp)
	{
		ArrowComp->SetRelativeLocation(TemplateLoc);
		ArrowComp->SetArrowColor(Color);
	}
}

void ARTSAIBlockActor::SetCustomProperties()
{
	SetArrowRelativeLocatin(EdgeLeftFrontComp, FVector(-BlockWidth / 2, TempEdgeLeftFrontLoc.Y, TempEdgeLeftFrontLoc.Z), ArrowColor);
	SetArrowRelativeLocatin(EdgeLeftBackComp, FVector(-BlockWidth / 2, TempEdgeLeftBackLoc.Y, TempEdgeLeftBackLoc.Z), ArrowColor);
	SetArrowRelativeLocatin(EdgeRightFrontComp, FVector(BlockWidth / 2, TempEdgeRightFrontLoc.Y, TempEdgeRightFrontLoc.Z), ArrowColor);
	SetArrowRelativeLocatin(EdgeRightBackComp, FVector(BlockWidth / 2, TempEdgeRightBackLoc.Y, TempEdgeRightBackLoc.Z), ArrowColor);
}

void ARTSAIBlockActor::FindAroundPawns(float DeltaSeconds)
{
	FindAroundPawnsTime += DeltaSeconds;

	if (FindAroundPawnsTime > FindAroundPawnsInterval)
	{
		FindAroundPawnsTime = 0.f;

		float CheckDist = BlockWidth * 1.5;

		AroundPawns.Reset();

		for (TActorIterator<APawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (FVector::Dist(ActorItr->GetActorLocation(), RootLocation) < CheckDist)
			{
				AroundPawns.Add(*ActorItr);
			}
		}
	}
}

void ARTSAIBlockActor::EnterClimbAreaCheck(float DeltaSeconds)
{
	EnterClimbAreaCheckTime += DeltaSeconds;

	if (EnterClimbAreaCheckTime > EnterClimbAreaCheckInterval)
	{
		EnterClimbAreaCheckTime = 0.f;

		for (APawn* Pawn : AroundPawns)
		{
			if (Pawn->IsValidLowLevelFast())
			{
				if (URTSAICrowdComponent* CrowdComp = URTSAIUtil::GetRTSAIComponent(Pawn))
				{
					ERTSAIState CurrState = CrowdComp->GetState();
					switch (CurrState)
					{
					case ERTSAIState::RTSAIS_GoForward:
					{
						if (FVector::Dist(RootLocation, Pawn->GetActorLocation()) <= BlockWidth / 2)
						{
							//检测：箭头起点到角色坐标的向量，与两个箭头向量的夹角，如果与 LeftEdgePass 的夹角小于 LeftEdge 的夹角，则说明进入了转向区域。
							FVector LeftArrowToPawn = (Pawn->GetActorLocation() - EdgeLeftLocation).GetSafeNormal();

							float LeftFrontAngle = EdgeLeftFrontDirection | LeftArrowToPawn;
							float LeftBackAngle = EdgeLeftBackDirection | LeftArrowToPawn;
							float LeftFrontRadian = FMath::RadiansToDegrees(acosf(LeftFrontAngle));
							float LeftBackRadian = FMath::RadiansToDegrees(acosf(LeftBackAngle));

							bool IsEntered = false;

							//如果左边检测没有进入攀爬区域，保险起见再检测一下右边箭头
							if (LeftFrontRadian < LeftBackRadian)
							{
								FVector RightArrowToPawn = Pawn->GetActorLocation() - EdgeRightLocation;
								float RightFrontRadian = acosf(EdgeRightFrontDirection | RightArrowToPawn);
								float RightBackRadian = acosf(EdgeRightBackDirection | RightArrowToPawn);
								if (RightFrontRadian >= RightBackRadian)
								{
									IsEntered = true;
								}
							}
							else
							{
								IsEntered = true;
							}

							if (IsEntered)
							{
								if (UPawnMovementComponent* MovementComp = Pawn->GetMovementComponent())
								{
									MovementComp->StopMovementImmediately();
								}

								//判断是右拐还是左拐
								float LeftEdgeDist = FVector::Dist(Pawn->GetActorLocation(), EdgeLeftLocation);
								float RightEdgeDist = FVector::Dist(Pawn->GetActorLocation(), EdgeRightLocation);

								FVector NewDirection = LeftEdgeDist > RightEdgeDist ? EdgeLeftFrontDirection : EdgeRightFrontDirection;
								CrowdComp->SetTurnDirection(NewDirection);
								CrowdComp->GetState(ERTSAIState::RTSAIS_Turning);
							}
						}
						break;
					}
					case ERTSAIState::RTSAIS_Turning:
					{
						FVector ActorLoc = Pawn->GetActorLocation();
						FVector ProjectPoint = UKismetMathLibrary::ProjectPointOnToPlane(ActorLoc, TurnDirecionMidPosition, TurnDirecionPanelNormal);

						if (FVector::Dist(ActorLoc, ProjectPoint) > BlockWidth / 2)
						{
							TurnFinishedEvent.Broadcast();
							CrowdComp->GetState(ERTSAIState::RTSAIS_GoForward);
							CrowdComp->SetDestDirection(CrowdComp->GetDestDirectionCache());
						}
						break;
					}
					}
				}
			}
		}
	}
}

