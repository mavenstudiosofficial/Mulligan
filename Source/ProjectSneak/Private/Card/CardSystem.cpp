// Fill out your copyright notice in the Description page of Project Settings.


#include "Card/CardSystem.h"
#include "Card/CardDatabase.h"
#include "Card/Card.h"
#include "SubSystems/GameManager.h"


void UCardSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency(UGameManager::StaticClass());

	auto gameInstance = GetWorld()->GetGameInstance();
	auto gameManager = gameInstance->GetSubsystem<UGameManager>();

	gameManager->AddPendingSystem(*this);

	UPSAssetManager::LoadAssetAsync(CARD_DB_ID, false, OnLoaded(_OnDatabaseLoaded));
}

void UCardSystem::Deinitialize()
{
	m_cardDatabase.Empty();
}

int UCardSystem::GetDeckSize() const
{
	return m_deckSize;
}

int UCardSystem::GetHandSize() const
{
	return m_handSize;
}

int UCardSystem::GetMaxInventoryAmount(FCardRarity rarity) const
{
	if (!m_isInited)
	{
		return 0;
	}

	auto intPtr = m_maxCardInvevtory.Find(rarity);
	return intPtr ? *intPtr : 0;
}

int UCardSystem::GetMaxDeckAmount(FCardRarity rarity) const
{
	if (!m_isInited)
	{
		return 0;
	}

	auto intPtr = m_maxCardDeck.Find(rarity);
	return intPtr ? *intPtr : 0;
}

void UCardSystem::GetAllCards(TArray<TSubclassOf<UCard>>& outCards) const
{
	if (!m_isInited)
	{
		return;
	}

	m_cardDatabase.GenerateValueArray(outCards);
}

void UCardSystem::GetAllCardsOfRarity(FCardRarity rarity, TArray<TSubclassOf<UCard>>& outCards) const
{
	if (!m_isInited)
	{
		return;
	}

	for (auto& kvp : m_cardDatabase)
	{
		auto card = kvp.Value.GetDefaultObject();
		if (card->rarity == rarity)
		{
			outCards.Add(kvp.Value);
		}
	}
}

UCard* UCardSystem::GetDefaultCard(FGuid cardId)
{
	if (!m_isInited)
	{
		return nullptr;
	}

	auto result = m_cardDatabase.Find(cardId);
	if (result)
	{
		return result->GetDefaultObject();
	}
	return nullptr;
}

void UCardSystem::_OnDatabaseLoaded(FGuid guid)
{
	UE_LOG(LogTemp, Display, TEXT("card database loaded:%s"), *guid.ToString());

	auto data = UPSAssetManager::GetLoadedAsset<UCardDatabase>(guid);

	if (data.assetPtr)
	{
		auto& database = *data.assetPtr;

		m_deckSize = database.deckSize;
		m_handSize = database.handSize;
		m_maxCardInvevtory = database.maxCardAmountInInvevtory;
		m_maxCardDeck = database.maxCardAmountInDeck;

		for (auto& item : database.cards)
		{
			auto defaultClass = item.GetDefaultObject();
			m_cardDatabase.Add(defaultClass->cardId, item);
		}
	}

	auto gameInstance = GetWorld()->GetGameInstance();
	auto gameManager = gameInstance->GetSubsystem<UGameManager>();

	gameManager->MakeSystemReady(*this);
	m_isInited = true;
}
