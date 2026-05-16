// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSettingsTypes.h"
#include "Card/Card.h"
#include "CardHolder.generated.h"

class UCard;
class UCardSystem;

UENUM(Blueprintable)
enum class ERemoveType : uint8
{
	FromDeck,
	FromHand,
	Both,
};

UCLASS(ClassGroup = (Card), meta = (BlueprintSpawnableComponent))
class PROJECTSNEAK_API UCardHolder : public UActorComponent
{
	GENERATED_BODY()

public:
	UCardHolder();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	bool HoldCard(FGuid cardId);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Hand")
	bool PlayCardByIndex(int cardIndex);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Hand")
	bool PlayCardByPtr(UCard* card);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Hand")
	bool DrawSelectedCard(UCard* card);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Hand")
	bool DrawCardFromDeck();

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Hand")
	TArray<int> DiscardOverflow();

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Inventory")
	int AddCardToInventory(FGuid cardId, int amount);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	bool AddCardToDeck(FGuid cardId, int& outIndex);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	bool RemoveCardFromDeck(int cardIndex);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	bool RemoveCard(UCard* card, ERemoveType removeType = ERemoveType::Both);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Inventory")
	int RemoveCardFromInventory(FGuid cardId, int amount);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	int GetDeckAmount(FGuid cardId);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Inventory")
	int GetInventoryAmount(FGuid cardId);

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Inventory")
	int GetAvailableAmount(FGuid cardId);

	UFUNCTION(BlueprintCallable, Reliable, Client, Category = "CardSystem | Deck")
	void SetLock (bool locked);

	UFUNCTION(BlueprintPure, Category = "CardSystem | Deck")
	bool IsCardClassMatch(FCardClassType cardType) const;

	UFUNCTION(BlueprintCallable, Category = "CardSystem | Deck")
	bool FindCardInDeck(FGuid cardId, int& cardIndex);

	void OnSwitchCharacter(EPlayerClass playerClass);

	UPROPERTY(BlueprintReadOnly, Category = "CardSystem | Deck")
	TArray<UCard*> deckCards;

	UPROPERTY(BlueprintReadOnly, Category = "CardSystem | Hand")
	TArray<UCard*> handCards;

	UPROPERTY(BlueprintReadOnly, Category = "CardSystem")
	bool isLocked;

	TMap<FGuid, int> cardInventory;

private:
	UCardSystem* m_cardSystem;
	EPlayerClass m_playerClass;

	UFUNCTION()
	void _InitInventory();
};
