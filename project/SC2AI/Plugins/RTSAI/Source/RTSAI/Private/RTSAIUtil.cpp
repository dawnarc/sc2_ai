// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSAIUtil.h"
#include "RTSAICrowdComponent.h"

URTSAICrowdComponent* URTSAIUtil::GetRTSAIComponent(AActor* Target)
{
	URTSAICrowdComponent* Ret = nullptr;
	TArray<UActorComponent*> Components = Target->GetComponentsByClass(URTSAICrowdComponent::StaticClass());

	if (Components.Num() > 0)
	{
		Ret = Cast<URTSAICrowdComponent>(Components[0]);
	}
	return Ret;
}

