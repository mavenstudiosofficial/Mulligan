// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card/Card.h"
#include "Shop/ShopDatabase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ShopSystem.generated.h"

class UCardHolder;
class UShopDatabase;

USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FShopItem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "ShopItem")
	int gemPrice;

	UPROPERTY(BlueprintReadOnly, Category = "ShopItem")
	int goldPrice;

	UPROPERTY(BlueprintReadOnly, Category = "ShopItem")
	UTexture2D* texture;

	UPROPERTY(BlueprintReadOnly, Category = "ShopItem")
	FShopItemRarity rarity;

	UPROPERTY(BlueprintReadOnly, Category = "ShopItem")
	bool isDefaultCard;

	int cardAmount;
	TMap<FCardRarity, int> gemFallback;
	TMap<FCardRarity, int> powerDistribution;
};

USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FShopOption
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "ShopOption")
	int gemAmount;

	UPROPERTY(BlueprintReadOnly, Category = "ShopOption")
	UCard* card;
};


UCLASS()
class PROJECTSNEAK_API UShopSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "ShopSystem")
	void GenerateShopItems(TArray<FShopItem>& result);

	UFUNCTION(BlueprintCallable, Category = "ShopSystem")
	void GetOptions(UCardHolder* cardHolder, const FShopItem& shopItem, TArray<FShopOption>& options);

private:
	UShopDatabase* m_shopData;

	const FGuid SHOP_DB_ID = FGuid("771D2A0F40BBB1973AC486994FBB35D3");

	void _OnDatabaseLoaded(FGuid guid);
};
