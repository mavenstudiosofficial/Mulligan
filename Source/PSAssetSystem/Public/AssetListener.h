// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AssetListener.generated.h"

PSASSETSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogPSAssetListener, Log, All);

UCLASS()
class PSASSETSYSTEM_API UAssetListener : public UObject
{
	GENERATED_BODY()
public:
	static void Startup();
	static void Shutdown();

private:
	static void _OnAssetAdded(const FAssetData& assetData);
	static void _OnAssetDeleted(const FAssetData& assetData);
};
