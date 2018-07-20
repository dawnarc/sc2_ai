// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SC2AICharacter.generated.h"

UENUM(BlueprintType)
enum class EGroup : uint8
{
	Ally,
	Enemy,
};

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

protected:

	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void CalcMovement(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
		void LerpRotate(float DeltaSeconds);

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EGroup Group;
};

