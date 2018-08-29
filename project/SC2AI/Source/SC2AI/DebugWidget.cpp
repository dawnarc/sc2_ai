// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UDebugWidget::UDebugWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}


bool UDebugWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* btn = Cast<UButton>(GetWidgetFromName("BtnTest")))
	{
		BtnTest = btn;

		FScriptDelegate Del;
		Del.BindUFunction(this, "OnBtnClicked");
		btn->OnClicked.Add(Del);
	}
}

void UDebugWidget::OnBtnClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString("+++++++++++++++++"));
}