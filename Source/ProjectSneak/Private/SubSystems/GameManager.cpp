// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystems/GameManager.h"

bool UGameManager::IsGameReady()
{
	return m_pendingSystemAmount <= 0;
}

void UGameManager::AddPendingSystem(USubsystem& system)
{
	m_pendingSystemAmount++;
}

void UGameManager::MakeSystemReady(USubsystem& system)
{
	m_pendingSystemAmount--;
	if (IsGameReady())
	{
		onGameReady.Broadcast();
		onGameReady.Clear();

		UE_LOG(LogTemp, Display, TEXT("Game Ready!"));
	}
}
