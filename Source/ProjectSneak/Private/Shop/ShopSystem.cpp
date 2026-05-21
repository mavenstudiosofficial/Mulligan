// Fill out your copyright notice in the Description page of Project Settings.
#include "Shop/ShopSystem.h"
#include "Shop/ShopDatabase.h"
#include "Card/Card.h"
#include "Card/CardHolder.h"
#include "Card/CardSystem.h"
#include "Data/PSAssetManager.h"
#include "SubSystems/GameManager.h"

void UShopSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency(UGameManager::StaticClass());                                                                                                                                                                                                                                                                                                                                

	auto gameInstance = GetWorld()->GetGameInstance();
	auto gameManager = gameInstance->GetSubsystem<UGameManager>();

	gameManager->AddPendingSystem(*this);

	UPSAssetManager::LoadAssetAsync(SHOP_DB_ID, false, OnLoaded(_OnDatabaseLoaded));
}

void UShopSystem::GenerateShopItems(TArray<FShopItem>& result)
{
	if (m_shopData == nullptr)
	{
		return;
	}

	for (auto& sourceItem : m_shopData->consumableItems)
	{
		FShopItem item = FShopItem();

		item.texture = sourceItem.texture;
		item.rarity = sourceItem.rarity;

		item.cardAmount = sourceItem.cardAmount;
		item.goldPrice = sourceItem.goldPrice;
		item.isDefaultCard = false;

		item.gemFallback = sourceItem.gemFallback;
		item.powerDistribution = sourceItem.powerDistribution;

		result.Add(item);
	}

	for (auto& sourceItem : m_shopData->defaultItems)
	{
		FShopItem item = FShopItem();

		item.texture = sourceItem.texture;
		item.rarity = sourceItem.rarity;

		item.cardAmount = sourceItem.cardAmount;
		item.gemPrice = sourceItem.gemPrice;
		item.isDefaultCard = true;

		item.powerDistribution = sourceItem.powerDistribution;

		result.Add(item);
	}
}

void UShopSystem::GetOptions(UCardHolder* cardHolder, const FShopItem& shopItem, TArray<FShopOption>& options)
{
	if (m_shopData == nullptr)
	{
		return;
	}

	auto gameInstance = GetGameInstance();
	auto cardSystem = gameInstance->GetSubsystem<UCardSystem>();

	int totalPower = 0;
	for (auto& kvp : shopItem.powerDistribution)
	{
		totalPower += kvp.Value;
	}

	TMap<FCardRarity, TArray<TSubclassOf<UCard>>> cardDict;
	for (auto& kvp : shopItem.powerDistribution)
	{
		auto rarity = kvp.Key;

		TArray<TSubclassOf<UCard>> cards;
		cardSystem->GetAllCardsOfRarity(rarity, cards);

		auto maxHold = cardSystem->GetMaxInventoryAmount(rarity);
		for (int32 i = cards.Num() - 1; i >= 0; i--)
		{
			auto libraryCard = cards[i].GetDefaultObject();
			if ((shopItem.isDefaultCard && libraryCard->type != FCardType::Default && libraryCard->type != FCardType::Class) ||
				(!shopItem.isDefaultCard && libraryCard->type != FCardType::Consumable))
			{
				cards.RemoveAtSwap(i);
				continue;
			}

			if (shopItem.isDefaultCard)
			{
				auto inventory = cardHolder->GetInventoryAmount(libraryCard->cardId);
				if (inventory >= maxHold)
				{
					cards.RemoveAtSwap(i);
					continue;
				}
			}
		}

		cardDict.Add(rarity, cards);
	}

	for (size_t i = 0; i < shopItem.cardAmount; i++)
	{
		FShopOption option = FShopOption();

		FCardRarity targetRartiy = FCardRarity::Basic;
		auto randomPower = FMath::RandRange(0, totalPower - 1);
		int currentPower = 0;
		for (auto& kvp : shopItem.powerDistribution)
		{
			currentPower += kvp.Value;
			if (currentPower > randomPower)
			{
				targetRartiy = kvp.Key;
				break;
			}
		}

		auto cardsPtr = cardDict.Find(targetRartiy);
		if (!cardsPtr)
		{
			option.gemAmount = 0;
			options.Add(option);
			continue;
		}

		TArray<TSubclassOf<UCard>> cards = *cardsPtr;
		if (cards.Num() <= 0)
		{
			auto gemFallback = shopItem.gemFallback.Find(targetRartiy);
			option.gemAmount = gemFallback == nullptr ? 0 : *gemFallback;
			options.Add(option);

			continue;
		}

		int randomIndex = FMath::RandRange(0, cards.Num() - 1);
		auto card = cards[randomIndex].GetDefaultObject();

		option.card = card;
		options.Add(option);
	}
}

void UShopSystem::_OnDatabaseLoaded(FGuid guid)
{
	UE_LOG(LogTemp, Display, TEXT("card database loaded:%s"), *guid.ToString());

	auto data = UPSAssetManager::GetLoadedAsset<UShopDatabase>(guid);

	m_shopData = data.assetPtr;

	auto gameInstance = GetWorld()->GetGameInstance();
	auto gameManager = gameInstance->GetSubsystem<UGameManager>();

	gameManager->MakeSystemReady(*this);
}
