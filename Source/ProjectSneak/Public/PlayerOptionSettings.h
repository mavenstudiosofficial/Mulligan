// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSettingsTypes.h"
#include "GameFramework/SaveGame.h"
#include "PlayerOptionSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FPlayerOptionSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    FIntPoint ScreenResolution;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    TEnumAsByte<EWindowMode::Type> WindowMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    float MouseSensitivity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    float MusicVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Options")
    float EffectsVolume;
};

UCLASS()
class PROJECTSNEAK_API UPSOptionsSave : public USaveGame 
{
    GENERATED_BODY()

public:
    UPSOptionsSave();

    UPROPERTY(BlueprintReadWrite, Category = "PlayerSave")
    FPlayerOptionSettings playerOptionsSettings;
};
