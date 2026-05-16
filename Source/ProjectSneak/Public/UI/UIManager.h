// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UIPanel.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Data/PSAssetManager.h"
#include "UI/UIPanelSheet.h"
#include <set>

#include "UIManager.generated.h"

USTRUCT()
struct FPanelFullscreen
{
	GENERATED_BODY()

public:

	FGuid PanelId;

	bool i_isFullscreen;
};

USTRUCT()
struct FPanelKey
{
	GENERATED_BODY()

public:

	FGuid panelId;

	APlayerController* player;

	FPanelKey() 
	{
		player = nullptr;
		panelId = FGuid();
	}

	FPanelKey(FGuid newPanelId, APlayerController* newPlayer)
	{
		panelId = newPanelId;
		player = newPlayer;
	}

	bool operator==(const FPanelKey& Other) const
	{
		return player == Other.player && panelId == Other.panelId;
	}
};

FORCEINLINE uint32 GetTypeHash(const FPanelKey& panelKey) 
{
	uint32 hash = GetTypeHash(panelKey.player);
	hash = HashCombine(hash, GetTypeHash(panelKey.panelId));
	return hash;
}
/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTSNEAK_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TMap<FGuid, TSubclassOf<class UUIPanel>> PanelClasses;

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	TArray<UUIPanel*> OpenPanels(FGuid i_PanelId, bool i_isFullscreen, bool i_Async = true);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	UUIPanel* OpenPanel(FGuid i_PanelId, APlayerController* player, bool i_Async = true);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	UUIPanel* OpenSharedPanel(FGuid i_PanelId, bool i_Async = true);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void ClosePanel(UUIPanel* i_Panel);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	bool HasPanel(FGuid i_PanelId, APlayerController* player);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	UUIPanel* GetPanel(FGuid i_PanelId, APlayerController* player);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void HidePanel(UUIPanel* i_UIPanel);

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void ShowPanel(FGuid i_PanelId, APlayerController* player);

protected:
	virtual void Initialize(FSubsystemCollectionBase& i_Collection) override;
	virtual void Deinitialize() override;

private:
	const FGuid PRELOAD_PANEL_ID = FGuid("5F5A93A040E017B4881C7E92541B3AB8");
	AssetRef<UUIPanelSheet> sheetRef = AssetRef<UUIPanelSheet>();

	UPROPERTY(Transient)
	TMap<FPanelKey, UUIPanel*> m_panelMap;

	UPROPERTY()
	TArray<UUIPanel*> m_sortedPanels;

	UPROPERTY()
	TMap<int32, FPanelFullscreen> m_panelsToLoad;
	int m_currentPanelOrder = 0;

	void OnPreloadPanelLoaded(FGuid guid);

	void OnOpenUIPanel(/*Blackboard Blackboard*/);

	void OnCloseUIPanel(/*Blackboard Blackboard*/);

	bool AddUIPanel(UUIPanel* i_UIPanel);

	void RefreshPanels();
};
