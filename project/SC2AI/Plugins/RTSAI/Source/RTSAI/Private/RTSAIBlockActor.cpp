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

	//�����ͷ��������
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

	//�������PostLoad������һ�μ�ͷ��RelativeLocation����ArrowComp�ڳ����༭���е���ʾ�����⣬ʹ�õĻ��ǹ��캯���е��������
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
							//��⣺��ͷ��㵽��ɫ�������������������ͷ�����ļнǣ������ LeftEdgePass �ļн�С�� LeftEdge �ļнǣ���˵��������ת������
							FVector LeftArrowToPawn = (Pawn->GetActorLocation() - EdgeLeftLocation).GetSafeNormal();

							float LeftFrontAngle = EdgeLeftFrontDirection | LeftArrowToPawn;
							float LeftBackAngle = EdgeLeftBackDirection | LeftArrowToPawn;
							float LeftFrontRadian = FMath::RadiansToDegrees(acosf(LeftFrontAngle));
							float LeftBackRadian = FMath::RadiansToDegrees(acosf(LeftBackAngle));

							bool IsEntered = false;

							//�����߼��û�н����������򣬱�������ټ��һ���ұ߼�ͷ
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

								//�ж����ҹջ������
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

