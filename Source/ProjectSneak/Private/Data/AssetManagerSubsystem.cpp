// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/AssetManagerSubsystem.h"

void UAssetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UPSAssetManager::LoadAssetAsync(PRELOAD_SHEET_ID, false, OnLoaded(_OnPreloadSheetLoaded));
}

void UAssetManagerSubsystem::_OnPreloadSheetLoaded(FGuid guid)
{
    UE_LOG(LogTemp, Display, TEXT("preload sheet loaded:%s"), *guid.ToString());

    auto preloadSheet = UPSAssetManager::GetLoadedAsset<UPreloadAssetSheet>(guid);
    preloadSheet.GrantRefTo(sheetRef);
    
    if (sheetRef.assetPtr)
    {
        auto sheet = sheetRef.assetPtr;
        auto basePreloadAmount = sheet->basePreloadConfig.Num();
        UE_LOG(LogTemp, Display, TEXT("base preload amount: %d"), basePreloadAmount);
    }
}

void UAssetManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}