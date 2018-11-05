// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSAIBlockActor.generated.h"

class UArrowComponent;
class USceneComponent;


UCLASS()
class RTSAI_API ARTSAIBlockActor : public AActor
{
	GENERATED_BODY()

public:

	ARTSAIBlockActor();

	DECLARE_EVENT(APawn, DeclareTurnFinishedEvent)

	DeclareTurnFinishedEvent& OnTurnFinished() { return TurnFinishedEvent; }

protected:

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostLoad() override;

private:

	//设置箭头的相对坐标
	FORCEINLINE void SetArrowRelativeLocatin(UArrowComponent* ArrowComp, const FVector& TemplateLoc, const FLinearColor& Color);

	//设置自定义属性
	FORCEINLINE void SetCustomProperties();

	//查找当前BlockActor附近的Pawn（预计算） @TODO inline
	void FindAroundPawns(float DeltaSeconds);

	//检测是否进入了转向区域 @TODO inline
	void EnterClimbAreaCheck(float DeltaSeconds);

protected:

	UPROPERTY(EditAnywhere)
		bool bEnabled = true;

	//检测延迟时间（用于保证各个ClimbingSplineActor检测逻辑的时间错开）
	float DelayCheckTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* CustomRootComp;

	//RootComponent的世界坐标
	FVector RootLocation;

	//判断是否进入攀爬区域的两个箭头组件（左边）
	UArrowComponent* EdgeLeftFrontComp;
	UArrowComponent* EdgeLeftBackComp;

	//左边箭头的世界坐标
	FVector EdgeLeftLocation;

	//判断是否进入攀爬区域的两个箭头的方向向量（左边）
	FVector EdgeLeftFrontDirection;
	FVector EdgeLeftBackDirection;

	//判断是否进入攀爬区域的两个箭头组件（右边）
	UArrowComponent* EdgeRightFrontComp;
	UArrowComponent* EdgeRightBackComp;

	//右边箭头的世界坐标
	FVector EdgeRightLocation;

	//判断是否进入攀爬区域的两个方向向量（右边）
	FVector EdgeRightFrontDirection;
	FVector EdgeRightBackDirection;

	//转向方向的平面法线单位向量
	FVector TurnDirecionPanelNormal;

	//转向方向的向量中点
	FVector TurnDirecionMidPosition;

	//攀爬区域检测的宽度
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		float BlockWidth = 200.f;

	//攀爬区域检测的深度（水平方向，从RootComponent到 ClimbLineComp 之间的距离）
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		float BlcokDepth = 50.f;

	//箭头颜色
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		FLinearColor ArrowColor = FLinearColor::Yellow;

	//左边箭头的模版transform
	const FVector TempEdgeLeftFrontLoc = FVector(-BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeLeftFrontRot = FRotator(0.f, 1.f, 0.f);
	const FVector TempEdgeLeftBackLoc = FVector(-BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeLeftBackRot = FRotator(0.f, -1.f, 0.f);

	//右边箭头的模版transform
	const FVector TempEdgeRightFrontLoc = FVector(BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeRightFrontRot = FRotator(0.f, 179.f, 0.f);
	const FVector TempEdgeRightBackLoc = FVector(BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeRightBackRot = FRotator(0.f, -179.f, 0.f);

	//当前ClimbingSplineActor附近的Pawn（预计算）
	TArray<APawn*> AroundPawns;

	float FindAroundPawnsInterval = 2.f;
	float FindAroundPawnsTime = 0.f;

	float EnterClimbAreaCheckInterval = 0.5f;
	float EnterClimbAreaCheckTime = 0.f;

	//ClimbAIState变化时的广播事件
	DeclareTurnFinishedEvent TurnFinishedEvent;

	//@TODO 测试
	FVector DebugPoint1;
	FVector DebugPoint2;
	FVector DebugPoint3;
	FVector DebugPoint4;
};
