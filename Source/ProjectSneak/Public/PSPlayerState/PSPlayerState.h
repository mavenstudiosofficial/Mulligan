#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include <PlayerSettingsTypes.h>
#include <PlayerOptionSettings.h>
#include "PSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSNEAK_API APSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyStateChanged, bool, isReady);

    UPROPERTY(BlueprintReadOnly, Category = "Player State")
    bool bIsReady = false;

    UPROPERTY(BlueprintReadOnly, Category = "Player State")
    FPlayerSetting PlayerSetting;

    UPROPERTY(BlueprintReadOnly, Category = "Player State")
    FPlayerSave PlayerSave;

    UPROPERTY(BlueprintAssignable, Category = "Player State")
    FOnReadyStateChanged OnReadyStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Player State", meta = (Tooltip = "Will Use GetPawn() if null"))
    void RefreshPlayerSetting(APawn* playerPawn = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Player State", meta = (Tooltip = "Will Use GetPawn() if null"))
    void RefreshPlayerSave(APawn* playerPawn = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    bool HasSaveData(int playerIndex);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SavePlayerData(int playerIndex);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void LoadPlayerData(int playerIndex);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void ReloadFromCache(const FPlayerSave& saveData, const FPlayerSetting& settingData);

    UFUNCTION(Reliable, Server, BlueprintCallable, Category = "Player State")
    void UploadReadyState(bool isReady);

    UFUNCTION(Reliable, Server, BlueprintCallable, Category = "Player State")
    void UploadPlayerSetting(const FPlayerSetting& setting);

    void OnSwitchCharacter(EPlayerClass playerClass);
    void ReloadFromSave(const FPlayerSave& saveData);
    void ReloadFromSetting(const FPlayerSetting& settingData);

    // Seamless travel persistence:
    virtual void CopyProperties(APlayerState* NewPlayerState) override;
    virtual void OverrideWith(APlayerState* OldPlayerState) override;

public:
    UPROPERTY(BlueprintReadOnly, Category = "Player State")
    FPlayerOptionSettings PlayerOptionSettings;

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void SavePlayerOptionSettings(int playerIndex, FPlayerOptionSettings newOptions);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void LoadPlayerOptionSettings(int playerIndex);

    UFUNCTION(BlueprintCallable, Category = "Player State")
    void ReloadFromOptionSettings(FPlayerOptionSettings playerOptions);
};
