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

	//���ü�ͷ���������
	FORCEINLINE void SetArrowRelativeLocatin(UArrowComponent* ArrowComp, const FVector& TemplateLoc, const FLinearColor& Color);

	//�����Զ�������
	FORCEINLINE void SetCustomProperties();

	//���ҵ�ǰBlockActor������Pawn��Ԥ���㣩 @TODO inline
	void FindAroundPawns(float DeltaSeconds);

	//����Ƿ������ת������ @TODO inline
	void EnterClimbAreaCheck(float DeltaSeconds);

protected:

	UPROPERTY(EditAnywhere)
		bool bEnabled = true;

	//����ӳ�ʱ�䣨���ڱ�֤����ClimbingSplineActor����߼���ʱ�����
	float DelayCheckTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* CustomRootComp;

	//RootComponent����������
	FVector RootLocation;

	//�ж��Ƿ�������������������ͷ�������ߣ�
	UArrowComponent* EdgeLeftFrontComp;
	UArrowComponent* EdgeLeftBackComp;

	//��߼�ͷ����������
	FVector EdgeLeftLocation;

	//�ж��Ƿ�������������������ͷ�ķ�����������ߣ�
	FVector EdgeLeftFrontDirection;
	FVector EdgeLeftBackDirection;

	//�ж��Ƿ�������������������ͷ������ұߣ�
	UArrowComponent* EdgeRightFrontComp;
	UArrowComponent* EdgeRightBackComp;

	//�ұ߼�ͷ����������
	FVector EdgeRightLocation;

	//�ж��Ƿ����������������������������ұߣ�
	FVector EdgeRightFrontDirection;
	FVector EdgeRightBackDirection;

	//ת�����ƽ�淨�ߵ�λ����
	FVector TurnDirecionPanelNormal;

	//ת����������е�
	FVector TurnDirecionMidPosition;

	//����������Ŀ��
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		float BlockWidth = 200.f;

	//�������������ȣ�ˮƽ���򣬴�RootComponent�� ClimbLineComp ֮��ľ��룩
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		float BlcokDepth = 50.f;

	//��ͷ��ɫ
	UPROPERTY(EditAnywhere, Category = ArrowInfo)
		FLinearColor ArrowColor = FLinearColor::Yellow;

	//��߼�ͷ��ģ��transform
	const FVector TempEdgeLeftFrontLoc = FVector(-BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeLeftFrontRot = FRotator(0.f, 1.f, 0.f);
	const FVector TempEdgeLeftBackLoc = FVector(-BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeLeftBackRot = FRotator(0.f, -1.f, 0.f);

	//�ұ߼�ͷ��ģ��transform
	const FVector TempEdgeRightFrontLoc = FVector(BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeRightFrontRot = FRotator(0.f, 179.f, 0.f);
	const FVector TempEdgeRightBackLoc = FVector(BlockWidth / 2, 0.f, 0.f);
	const FRotator TempEdgeRightBackRot = FRotator(0.f, -179.f, 0.f);

	//��ǰClimbingSplineActor������Pawn��Ԥ���㣩
	TArray<APawn*> AroundPawns;

	float FindAroundPawnsInterval = 2.f;
	float FindAroundPawnsTime = 0.f;

	float EnterClimbAreaCheckInterval = 0.5f;
	float EnterClimbAreaCheckTime = 0.f;

	//ClimbAIState�仯ʱ�Ĺ㲥�¼�
	DeclareTurnFinishedEvent TurnFinishedEvent;

	//@TODO ����
	FVector DebugPoint1;
	FVector DebugPoint2;
	FVector DebugPoint3;
	FVector DebugPoint4;
};
