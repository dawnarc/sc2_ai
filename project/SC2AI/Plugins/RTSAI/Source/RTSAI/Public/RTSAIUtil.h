// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RTSAIUtil.generated.h"

class URTSAICrowdComponent;

/**
 * 
 */
UCLASS()
class RTSAI_API URTSAIUtil : public UObject
{
	GENERATED_BODY()
	
public:
	
	static URTSAICrowdComponent* GetRTSAIComponent(AActor* Target);
	
};
