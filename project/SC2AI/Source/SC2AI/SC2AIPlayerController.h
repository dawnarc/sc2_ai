// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SC2AIPlayerController.generated.h"

class UDebugWidget;
class ASC2AICharacter;

UCLASS()
class ASC2AIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASC2AIPlayerController();

	ASC2AICharacter* GetSelectedCharacter() {return SelectedCharacter;}

	void ResetSelectedCharacter();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void OnMouseClick();

	void AllAgentsStopMove();

	void SpawnAgentBegin();

protected:

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UDebugWidget> DebugWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* DebugHighlightMaterial;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* DefaultMaterial;

	UDebugWidget* DebugWidget;

private:

	ASC2AICharacter* SelectedCharacter;
};


