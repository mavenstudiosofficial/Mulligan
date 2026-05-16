// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "PreloadAssetSheet.h"
#include "PSAssetManager.h"

#include "AssetManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSNEAK_API UAssetManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) final;
	virtual void Deinitialize() final;

private:
	void _OnPreloadSheetLoaded(FGuid guid);
	const FGuid PRELOAD_SHEET_ID = FGuid("5EB487D448A32AA6D6C60888C742CBEF");

	AssetRef<UPreloadAssetSheet> sheetRef;
};

