// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ManagedAsset.h"
#include "Card.generated.h"


UENUM(BlueprintType)
enum class FCardType : uint8
{
	Default,
	Consumable,
	Temporary,
	Class
};


UENUM(BlueprintType)
enum class FCardRarity : uint8
{
	Basic,
	Common,
	Uncommon,
	Rare,
	Legendary
};

UENUM(BlueprintType)
enum class FCardClassType : uint8
{
	NoClass,
	Strange,
	Strength,
	Sneak,
	Support,
	Artifact
};

class UCardHolder;

UCLASS(Blueprintable, Meta = (ShowWorldContextPin))
class PROJECTSNEAK_API UCard : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Card | Events")
	void OnCardPlayed(AActor* holderActor, int cardIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Card | Events")
	void OnCardAdded(AActor* holderActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Card | Events")
	void OnCardDrawed(AActor* holderActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Card | Events")
	void OnCardRemoved(AActor* holderActor, int cardIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Card | Events")
	void OnCardDiscarded(AActor* holderActor, int cardIndex);

	UFUNCTION(BlueprintCallable, Category = "Card | Interfaces")
	bool PlaySelf();

	UFUNCTION(BlueprintCallable, Category = "Card | Interfaces")
	bool DrawSelf();

	void PostInitProperties() override;

#define LOCTEXT_NAMESPACE "Card"

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Display")
	FText name = LOCTEXT("CardName", "<CardName>");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Display", meta = (MultiLine = "true"))
	FText description = LOCTEXT("CardDescription", "<CardDescription>");

#undef LOCTEXT_NAMESPACE

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Art")
	UTexture2D* texture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	FCardType type = FCardType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	FCardRarity rarity = FCardRarity::Basic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	FCardClassType classType = FCardClassType::NoClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	bool usable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	bool couldDiscard = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	int32 slotOccupied = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card | Base Config")
	FGuid cardId;

	UPROPERTY(BlueprintReadOnly, Category = "Card | Runtime")
	TObjectPtr<UCardHolder> holder;

	UFUNCTION(BlueprintCallable, Category = "Card | Interfaces")
	UTexture2D* GetTexture();
};
