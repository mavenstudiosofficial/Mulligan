// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ManagedAsset.h"
#include "Card.h"
#include "CardDatabase.generated.h"


UCLASS()
class PROJECTSNEAK_API UCardDatabase : public UManagedDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Card System")
	int deckSize;

	UPROPERTY(EditAnywhere, Category = "Card System")
	int handSize;

	UPROPERTY(EditAnywhere, Category = "Card System")
	TMap<FCardRarity, int> maxCardAmountInInvevtory;

	UPROPERTY(EditAnywhere, Category = "Card System")
	TMap<FCardRarity, int> maxCardAmountInDeck;

	UPROPERTY(EditAnywhere, Category = "Cards")
	TSet<TSubclassOf<UCard>> cards;
};
