#pragma once

#include "CoreMinimal.h"
#include "PlayerSettingsTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
    Default        UMETA(DisplayName="Strange"),
    Scout       UMETA(DisplayName="Strength"),
    Hacker      UMETA(DisplayName="Sneak"),
    Enforcer    UMETA(DisplayName="Support"),
};

USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FCharacterSave
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Save")
    int32 xp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Save")
    TArray<FGuid> deckPreset;
};

// The data saved in local
USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FPlayerSave
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Save")
    EPlayerClass selectedCharacter = EPlayerClass::Default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Save")
    int32 coins; 
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Save")
    int32 gems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Save")
    TMap<FGuid, int> cardInventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Save")
    TMap<EPlayerClass, FCharacterSave> characterSet;
};

// The data saved on server, temp use for level traveling
USTRUCT(BlueprintType)
struct PROJECTSNEAK_API FPlayerSetting
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Save")
    EPlayerClass character = EPlayerClass::Default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Save")
    TArray<FGuid> deck;
};