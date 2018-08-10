// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSAIUtil.h"
#include "RTSCrowdAIComponent.h"

URTSCrowdAIComponent* URTSAIUtil::GetRTSAIComponent(AActor* Target)
{
	URTSCrowdAIComponent* Ret = nullptr;
	TArray<UActorComponent*> Components = Target->GetComponentsByClass(URTSCrowdAIComponent::StaticClass());

	if (Components.Num() > 0)
	{
		Ret = Cast<URTSCrowdAIComponent>(Components[0]);
	}
	return Ret;
}

