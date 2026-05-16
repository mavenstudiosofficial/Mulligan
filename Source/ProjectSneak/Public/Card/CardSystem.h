// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "Card.h"
#include "Data/PSAssetManager.h"
#include "CardDatabase.h"

#include "CardSystem.generated.h"


UCLASS()
class PROJECTSNEAK_API UCardSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	int GetDeckSize() const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	int GetHandSize() const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	int GetMaxInventoryAmount(FCardRarity rarity) const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	int GetMaxDeckAmount(FCardRarity rarity) const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	void GetAllCards(TArray<TSubclassOf<UCard>>& outCards) const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	void GetAllCardsOfRarity(FCardRarity rarity, TArray<TSubclassOf<UCard>>& outCards) const;

	UFUNCTION(BlueprintPure, Category = "CardSystem")
	UCard* GetDefaultCard(FGuid cardId);

private:
	bool m_isInited = false;
	TMap<FGuid, TSubclassOf<UCard>> m_cardDatabase;

	int m_deckSize;
	int m_handSize;
	TMap<FCardRarity, int> m_maxCardInvevtory;
	TMap<FCardRarity, int> m_maxCardDeck;

	const FGuid CARD_DB_ID = FGuid("615E8C6E44418F2716C6C189C5F0D68C");
	

	void _OnDatabaseLoaded(FGuid guid);
};
