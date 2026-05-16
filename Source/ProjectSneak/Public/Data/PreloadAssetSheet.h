// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ManagedAsset.h"
#include "PreloadAssetSheet.generated.h"

USTRUCT()
struct FPreloaderAssetConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FGuid assetId;

	UPROPERTY(EditAnywhere)
	FPrimaryAssetType assetType;
};


USTRUCT()
struct FPreloaderLevelConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FPreloaderAssetConfig> assets;
};


UCLASS()
class PROJECTSNEAK_API UPreloadAssetSheet : public UManagedDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditInstanceOnly)
	TArray<FPreloaderAssetConfig> basePreloadConfig;

	UPROPERTY(EditInstanceOnly)
	TMap<FName, FPreloaderLevelConfig> levelConfigs;
};
