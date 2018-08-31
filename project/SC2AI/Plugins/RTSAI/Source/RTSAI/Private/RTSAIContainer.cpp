// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSAIContainer.h"

TArray<APawn*> URTSAIContainer::AgentList = TArray<APawn*>();

void URTSAIContainer::AddAgent(APawn* Pawn)
{
	AgentList.Add(Pawn);
}

void URTSAIContainer::RemoveAgent(APawn* Pawn)
{
	AgentList.Remove(Pawn);
}

const TArray<APawn*>& URTSAIContainer::GetAgentList()
{
	return AgentList;
}
