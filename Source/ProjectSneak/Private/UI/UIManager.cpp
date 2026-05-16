// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UIManager.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Data/PSAssetManager.h"

void UUIManager::Initialize(FSubsystemCollectionBase& i_Collection)
{
	UPSAssetManager::LoadAssetAsync(PRELOAD_PANEL_ID, false, OnLoaded(OnPreloadPanelLoaded));
	UE_LOG(LogTemp, Display, TEXT("Initialized UIManager"));
}

void UUIManager::Deinitialize()
{

}

void UUIManager::OnPreloadPanelLoaded(FGuid guid)
{
	auto preloadSheet = UPSAssetManager::GetLoadedAsset<UUIPanelSheet>(PRELOAD_PANEL_ID);
	preloadSheet.GrantRefTo(sheetRef);

	if (sheetRef.assetPtr)
	{
		auto sheet = sheetRef.assetPtr;
		for (int p = 0; p < sheet->basePreloadPanelsConfig.Num(); p++)
		{
			PanelClasses.Add(sheet->basePreloadPanelsConfig[p].panelId, sheet->basePreloadPanelsConfig[p].panel->GetClass());
		}

		for (int i = 0; i < m_panelsToLoad.Num(); i++)
		{
			OpenPanels(m_panelsToLoad[i].PanelId, m_panelsToLoad[i].i_isFullscreen);
		}
	}
}

TArray<UUIPanel*> UUIManager::OpenPanels(FGuid i_PanelId, bool i_isFullscreen, bool i_Async)
{
	if (!sheetRef.assetPtr)
	{
		m_panelsToLoad.Add(m_panelsToLoad.Num(), { i_PanelId, i_isFullscreen });
		return TArray<UUIPanel*>();
	}

	if (!i_isFullscreen)
	{
		UWorld* World = GetWorld();

		for (ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			if (!LocalPlayer)
				continue;

			APlayerController* PlayerController = LocalPlayer->GetPlayerController(World);

			if (!PlayerController)
				continue;

			UUIPanel* Panel = OpenPanel(i_PanelId, PlayerController, i_Async);

			if (Panel)
			{
				Panel->SetOwningLocalPlayer(LocalPlayer);
				Panel->AddToPlayerScreen();
			}
		}
	}
	else
	{
		UWorld* World = GetWorld();
		ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
		APlayerController* Controller = LocalPlayer->GetPlayerController(World);

		UUIPanel* Panel = OpenPanel(i_PanelId, Controller, i_Async);

		Panel->SetOwningLocalPlayer(LocalPlayer);
		Panel->AddToViewport();
	}

	return m_sortedPanels;
}

UUIPanel* UUIManager::OpenPanel(FGuid i_PanelId, APlayerController* player, bool i_Async)
{
	if (!player || !player->IsLocalController())
	{
		return nullptr;
	}

	UUIPanel* Panel = nullptr;

	Panel = CreateWidget<UUIPanel>(player, PanelClasses[i_PanelId]);

	if (!Panel)
	{
		return nullptr;
	}

	Panel->PanelId = i_PanelId;

	// Register the new panel
	m_panelMap.Add({ i_PanelId, player }, Panel);
	AddUIPanel(Panel);

	Panel->InitPanel();
	Panel->SetVisibility(ESlateVisibility::Visible);

	return Panel;
}

UUIPanel* UUIManager::OpenSharedPanel(FGuid i_PanelId, bool i_Async)
{
	UUIPanel* Panel = nullptr;

	Panel = CreateWidget<UUIPanel>(GetWorld(), PanelClasses[i_PanelId]);

	if (!Panel)
	{
		return nullptr;
	}

	Panel->PanelId = i_PanelId;

	// Register the new panel
	m_panelMap.Add({ i_PanelId, nullptr }, Panel);
	AddUIPanel(Panel);

	Panel->InitPanel();
	Panel->SetVisibility(ESlateVisibility::Visible);

	return Panel;
}

void UUIManager::ClosePanel(UUIPanel* i_Panel)
{
	m_sortedPanels.Remove(i_Panel);

	i_Panel->Close();

	GEngine->ForceGarbageCollection(true);
}

bool UUIManager::HasPanel(FGuid i_PanelId, APlayerController* player)
{
	return m_panelMap.Contains({ i_PanelId, player });
}

UUIPanel* UUIManager::GetPanel(FGuid i_PanelId, APlayerController* player)
{
	//UE_LOG(LogTemp, Warning, TEXT("GetPanel called on UIManager at: %p, Map has %d entries"), this, m_panelMap.Num());
	if (HasPanel(i_PanelId, player))
	{
		return *m_panelMap.Find({ i_PanelId, player });
	}
	return nullptr;
}

void UUIManager::OnOpenUIPanel(/*Blackboard Blackboard*/)
{

}

void UUIManager::OnCloseUIPanel(/*Blackboard Blackboard*/)
{

}

bool UUIManager::AddUIPanel(UUIPanel* i_UIPanel)
{
	if (!i_UIPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to add a null UIPanel"));
		return false;
	}

	i_UIPanel->PanelOrder = m_currentPanelOrder++;

	/*if (!m_sortedPanels)
	{
		m_sortedPanels = MakeShared<TArray<UUIPanel*>>();
	}*/

	m_sortedPanels.Add(i_UIPanel);

	m_sortedPanels.Sort([](const UUIPanel& A, const UUIPanel& B)
		{
			return A.PanelOrder < B.PanelOrder;
		});

	RefreshPanels();

	return true;
}

void UUIManager::RefreshPanels()
{
	m_sortedPanels.Sort([](const UUIPanel& A, const UUIPanel& B)
		{
			return A.PanelOrder > B.PanelOrder;
		});

	bool hideOthers = false;
	for (UUIPanel* panel : m_sortedPanels)
	{
		if (hideOthers)
		{
			panel->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		panel->Show();

		if ((panel->PanelBehavior & EUIPanelBehavior::HideOthersPanel) == EUIPanelBehavior::HideOthersPanel)
		{
			hideOthers = true;
		}
	}
}

void UUIManager::HidePanel(UUIPanel* i_UIPanel)
{
	if (i_UIPanel)
		i_UIPanel->Hide();
}

void UUIManager::ShowPanel(FGuid i_PanelId, APlayerController* player)
{
	UUIPanel* UIPanel = GetPanel(i_PanelId, player);
	if (UIPanel)
		UIPanel->Show();
}
