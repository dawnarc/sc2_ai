// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class SC2AI_API UDebugWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	UDebugWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TxtDebug;

protected:

	bool Initialize() override;

	void NativeConstruct() override;

	UFUNCTION()
	void OnBtnClicked();

private:

	UButton* BtnTest = nullptr;
	
};
