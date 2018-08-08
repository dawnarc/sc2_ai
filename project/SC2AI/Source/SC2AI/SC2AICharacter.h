// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SC2AIComponent.h"
#include "SC2AICharacter.generated.h"

class USC2AIComponent;

UCLASS(Blueprintable)
class ASC2AICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASC2AICharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void SetGroup(EGroup GroupType);

	void GetOverlapCount(int& FwdCount, int& LeftCount, int& RightCount, int& FwdLeftCount, int& FwdRightCount);

	void SetCollisionVisible(bool IsVisible);

	void SetDestDirection(const FVector& Direction);

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//Key: overlap count in box; Value:current box direction
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TMap<int32, FVector> DirectionMap;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USC2AIComponent* SC2AIComponent;
};

