// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ManagedAsset.h"
#include "UI/UIPanel.h"

#include "UIPanelSheet.generated.h"

USTRUCT()
struct FPreloaderPanelAssetConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGuid panelId;

	UPROPERTY(EditAnywhere)
	UUIPanel* panel;
};

/**
 *
 */
UCLASS()
class PROJECTSNEAK_API UUIPanelSheet : public UManagedDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FPreloaderPanelAssetConfig> basePreloadPanelsConfig;
};
