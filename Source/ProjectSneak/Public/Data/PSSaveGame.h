// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSettingsTypes.h"
#include "GameFramework/SaveGame.h"
#include "PSSaveGame.generated.h"

/**
 *
 */
UCLASS()
class PROJECTSNEAK_API UPSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPSSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "PlayerSave")
	FPlayerSave playerSave;
};
