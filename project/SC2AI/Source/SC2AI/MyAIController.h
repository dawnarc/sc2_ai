// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SC2AI_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AMyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
