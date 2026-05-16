// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/Wallet.h"

UWallet::UWallet()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWallet::GainGem(int amount)
{
	m_gem += amount;
}

void UWallet::GainGold(int amount)
{
	m_gold += amount;
}

int UWallet::GetGemAmount()
{
	return m_gem;
}

int UWallet::GetGoldAmount()
{
	return m_gold;
}

bool UWallet::HasEnoughGem(int amount)
{
	return m_gem >= amount;
}

bool UWallet::HasEnoughGold(int amount)
{
	return m_gold >= amount;
}

bool UWallet::TryUseGem(int amount)
{
	if (HasEnoughGem(amount))
	{
		m_gem -= amount;
		return true;
	}
	return false;
}

bool UWallet::TryUseGold(int amount)
{
	if (HasEnoughGold(amount))
	{
		m_gold -= amount;
		return true;
	}
	return false;
}


// Called when the game starts
void UWallet::BeginPlay()
{
	Super::BeginPlay();
}

