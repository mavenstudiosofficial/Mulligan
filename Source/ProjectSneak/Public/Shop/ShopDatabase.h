// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ManagedAsset.h"
#include "Card/Card.h"
#include "ShopDatabase.generated.h"

UENUM(BlueprintType)
enum class FShopItemRarity : uint8
{
	Basic,
	Rare,
	Legendary
};

USTRUCT()
struct FDefaultCardPack
{
	GENERATED_BODY()
public:
	UPROPERTY(EditInstanceOnly, Category = "CardPack | Art")
	UTexture2D* texture;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	FShopItemRarity rarity;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	int gemPrice = 10;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	int cardAmount = 3;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	TMap<FCardRarity, int> powerDistribution;
};

USTRUCT()
struct FConsumableCardPack
{
	GENERATED_BODY()
public:
	UPROPERTY(EditInstanceOnly, Category = "CardPack | Art")
	UTexture2D* texture;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	FShopItemRarity rarity;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	int goldPrice = 10;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	int cardAmount = 3;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	TMap<FCardRarity, int> gemFallback;

	UPROPERTY(EditInstanceOnly, Category = "CardPack | BaseConfig")
	TMap<FCardRarity, int> powerDistribution;
};

UCLASS()
class PROJECTSNEAK_API UShopDatabase : public UManagedDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditInstanceOnly, Category = "Shop | Items")
	TArray<FDefaultCardPack> defaultItems;

	UPROPERTY(EditInstanceOnly, Category = "Shop | Items")
	TArray<FConsumableCardPack> consumableItems;
};
