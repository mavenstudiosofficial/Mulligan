// Fill out your copyright notice in the Description page of Project Settings.


#include "Card/CardHolder.h"
#include "Card/CardSystem.h"
#include "SubSystems/GameManager.h"

UCardHolder::UCardHolder()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCardHolder::BeginPlay()
{
	Super::BeginPlay();

	auto gameInstance = GetWorld()->GetGameInstance();

	auto gameManager = gameInstance->GetSubsystem<UGameManager>();
	if (gameManager->IsGameReady())
	{
		_InitInventory();
	}
	else
	{
		gameManager->onGameReady.AddDynamic(this, &UCardHolder::_InitInventory);
	}
}

bool UCardHolder::HoldCard(FGuid cardId)
{
	for (auto& card : handCards)
	{
		if (card->cardId == cardId)
		{
			return true;
		}
	}

	for (auto& card : deckCards)
	{
		if (card->cardId == cardId)
		{
			return true;
		}
	}

	return false;
}

void UCardHolder::_InitInventory()
{
	auto gameInstance = GetWorld()->GetGameInstance();
	cardInventory = {};

	m_cardSystem = gameInstance->GetSubsystem<UCardSystem>();
	if (m_cardSystem)
	{
		TArray<TSubclassOf<UCard>> allCards;
		m_cardSystem->GetAllCards(allCards);

		for (auto& card : allCards)
		{
			auto defaultClass = card.GetDefaultObject();
			cardInventory.Add(defaultClass->cardId);
		}
	}
}

bool UCardHolder::PlayCardByIndex(int cardIndex)
{
	if (isLocked)
	{
		return false;
	}

	if (!handCards.IsValidIndex(cardIndex))
	{
		return false;
	}

	auto holder = GetOwner();

	auto card = handCards[cardIndex];
	if (!card->usable)
	{
		return false;
	}

	card->OnCardPlayed(holder, cardIndex);
	handCards.RemoveAt(cardIndex);

	if (card->type == FCardType::Consumable)
	{
		RemoveCardFromInventory(card->cardId, 1);
	}

	return true;
}

bool UCardHolder::PlayCardByPtr(UCard* card)
{
	if (isLocked)
	{
		return false;
	}

	auto handSize = handCards.Num();
	int index = -1;

	for (int i = 0; i < handSize; i++)
	{
		if (handCards[i] == card)
		{
			index = i;
			break;
		}
	}

	if (index >= 0)
	{
		return PlayCardByIndex(index);
	}
	return false;
}

bool UCardHolder::DrawSelectedCard(UCard* card)
{
	if (isLocked)
	{
		return false;
	}

	int deckCardIndex = deckCards.IndexOfByKey(card);
	if (deckCardIndex == INDEX_NONE)
	{
		return false;
	}

	auto holder = GetOwner();

	deckCards.RemoveAt(deckCardIndex);

	handCards.Add(card);
	card->OnCardDrawed(holder);

	return true;
}

bool UCardHolder::DrawCardFromDeck()
{
	if (isLocked)
	{
		return false;
	}

	if (deckCards.Num() <= 0)
	{
		return false;
	}

	auto holder = GetOwner();

	auto randomIndex = FMath::RandRange(0, deckCards.Num() - 1);
	auto card = deckCards[randomIndex];
	deckCards.RemoveAt(randomIndex);

	handCards.Add(card);
	card->OnCardDrawed(holder);

	return true;
}

TArray<int> UCardHolder::DiscardOverflow()
{
	int handSize = m_cardSystem->GetHandSize();
	TArray<int> discardedIDs = {};

	if (isLocked)
	{
		return discardedIDs;
	}

	if (handCards.Num() <= handSize)
	{
		return discardedIDs;
	}

	auto holder = GetOwner();

	int index = 0;
	while (handCards.Num() > handSize && index < handCards.Num())
	{ // While handcards is bigger than handsize and checked card isn't past the last card in hand
		auto card = handCards[index];
		if (card->couldDiscard)
		{ // Check if card is discardable. If it is, discard it
			int handIndex = index + discardedIDs.Num();
			discardedIDs.Add(handIndex);

			handCards.RemoveAt(index);

			card->OnCardDiscarded(holder, handIndex);
		}
		else
		{ // otherwise skip card
			index++;
		}
	}

	discardedIDs.Sort([](const int& A, const int& B) { return A > B; });

	return discardedIDs;
}

int UCardHolder::AddCardToInventory(FGuid cardId, int amount)
{
	auto defaultCard = m_cardSystem->GetDefaultCard(cardId);
	if (!defaultCard || defaultCard->type == FCardType::Temporary)
	{
		return 0;
	}

	auto cardAmountPtr = cardInventory.Find(cardId);

	auto cardAmount = cardAmountPtr ? *cardAmountPtr : 0;
	auto maxAmount = defaultCard->type == FCardType::Consumable ?
		999999 : m_cardSystem->GetMaxInventoryAmount(defaultCard->rarity);

	auto newAmount = FMath::Clamp(cardAmount + amount, 0, maxAmount);

	if (cardInventory.Contains(cardId))
	{
		cardInventory[cardId] = newAmount;
	}
	else
	{
		cardInventory.Add(cardId, newAmount);
	}

	return newAmount - cardAmount;
}

bool UCardHolder::AddCardToDeck(FGuid cardId, int& outIndex)
{
	if (deckCards.Num() >= m_cardSystem->GetDeckSize())
	{
		return false;
	}

	auto defaultCard = m_cardSystem->GetDefaultCard(cardId);
	if (!defaultCard)
	{
		return false;
	}

	if (GetAvailableAmount(cardId) <= 0)
	{
		return false;
	}

	auto maxDeckAmount = m_cardSystem->GetMaxDeckAmount(defaultCard->rarity);

	if (GetDeckAmount(defaultCard->cardId) >= maxDeckAmount)
	{
		return false;
	}

	auto holder = GetOwner();

	auto newCard = NewObject<UCard>(this, defaultCard->GetClass());
	newCard->cardId = defaultCard->cardId;
	newCard->holder = this;

	deckCards.Add(newCard);

	if (!isLocked)
	{
		newCard->OnCardAdded(holder);
	}

	outIndex = deckCards.Num() - 1;

	return true;
}

bool UCardHolder::RemoveCardFromDeck(int cardIndex)
{
	if (!deckCards.IsValidIndex(cardIndex))
	{
		return false;
	}

	auto holder = GetOwner();

	auto card = deckCards[cardIndex];
	if (!isLocked)
	{
		card->OnCardRemoved(holder, -1);
	}
	deckCards.RemoveAt(cardIndex);

	return true;
}

bool UCardHolder::RemoveCard(UCard* card, ERemoveType removeType)
{
	auto holder = GetOwner();
	auto world = GetWorld();

	if (removeType == ERemoveType::Both || removeType == ERemoveType::FromHand)
	{
		auto handIndex = handCards.IndexOfByKey(card);
		if (handIndex != INDEX_NONE)
		{
			if (!isLocked)
			{
				card->OnCardRemoved(holder, handIndex);
			}
			handCards.RemoveAt(handIndex);
			return true;
		}
	}

	if (removeType == ERemoveType::Both || removeType == ERemoveType::FromHand)
	{
		auto deckIndex = deckCards.IndexOfByKey(card);
		if (deckIndex != INDEX_NONE)
		{
			if (!isLocked)
			{
				card->OnCardRemoved(holder, -1);
			}
			deckCards.RemoveAt(deckIndex);
			return true;
		}
	}

	return false;
}

int UCardHolder::RemoveCardFromInventory(FGuid cardId, int amount)
{
	auto inventoryPtr = cardInventory.Find(cardId);
	if (!inventoryPtr)
	{
		return 0;
	}

	int inventory = *inventoryPtr;
	int newInventory = FMath::Clamp(inventory - amount, 0, inventory);
	cardInventory[cardId] = newInventory;

	return newInventory - inventory;
}

int UCardHolder::GetDeckAmount(FGuid cardId)
{
	int result = 0;
	for (auto& card : deckCards)
	{
		if (card->cardId == cardId)
		{
			result++;
		}
	}
	return result;
}

int UCardHolder::GetInventoryAmount(FGuid cardId)
{
	auto amountPtr = cardInventory.Find(cardId);
	if (amountPtr)
	{
		return *amountPtr;
	}
	return 0;
}

int UCardHolder::GetAvailableAmount(FGuid cardId)
{
	auto defaultCard = m_cardSystem->GetDefaultCard(cardId);

	if (!defaultCard || !IsCardClassMatch(defaultCard->classType))
	{
		return 0;
	}

	int cardAmount;
	if (defaultCard->type == FCardType::Temporary)
	{
		cardAmount = m_cardSystem->GetMaxDeckAmount(defaultCard->rarity);
	}
	else
	{
		cardAmount = GetInventoryAmount(cardId);
	}

	int usedAmount = 0;
	for (auto& card : deckCards)
	{
		if (card->cardId == cardId)
		{
			usedAmount++;
		}
	}

	for (auto& card : handCards)
	{
		if (card->cardId == cardId)
		{
			usedAmount++;
		}
	}

	return FMath::Clamp(cardAmount - usedAmount, 0, cardAmount);
}

void UCardHolder::SetLock_Implementation(bool locked)
{
	this->isLocked = locked;
}

bool UCardHolder::IsCardClassMatch(FCardClassType cardType) const
{
	if ((cardType == FCardClassType::Strange && m_playerClass != EPlayerClass::Default) ||
		(cardType == FCardClassType::Sneak && m_playerClass != EPlayerClass::Hacker) ||
		(cardType == FCardClassType::Strength && m_playerClass != EPlayerClass::Scout) ||
		(cardType == FCardClassType::Support && m_playerClass != EPlayerClass::Enforcer))
	{
		return false;
	}

	return true;
}

bool UCardHolder::FindCardInDeck(FGuid cardId, int& cardIndex)
{
	for (int i = 0; i < deckCards.Num(); i++)
	{
		if (deckCards[i]->cardId == cardId)
		{
			cardIndex = i;
			return true;
		}
	}

	return false;
}

void UCardHolder::OnSwitchCharacter(EPlayerClass playerClass)
{
	m_playerClass = playerClass;

	for (int i = deckCards.Num() - 1; i >= 0; i--)
	{
		auto card = deckCards[i];
		if (!IsCardClassMatch(card->classType))
		{
			RemoveCardFromDeck(i);
		}
	}
}

