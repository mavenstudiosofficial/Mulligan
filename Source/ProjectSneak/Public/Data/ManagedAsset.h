// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ManagedAsset.generated.h"


//class GuidUtils
//{
//public:
//	static FString ToString(int64 value);
//	static bool IsValid(int64 value);
//
//	static int64 GenerateNew();
//};


namespace DataAssetType
{
	const FPrimaryAssetType PSDataAsset = TEXT("PSDataAsset");
	const FPrimaryAssetType PSBlueprintAsset = TEXT("PSBlueprintAsset");
};

UCLASS()
class PROJECTSNEAK_API UManagedDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Asset Config")
	FGuid guid;

	UPROPERTY(VisibleAnywhere, Category = "Asset Config")
	FPrimaryAssetType assetType = DataAssetType::PSDataAsset;

	void PostInitProperties() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

UCLASS()
class PROJECTSNEAK_API UManagedBlueprintAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Asset Config")
	FGuid guid;

	UPROPERTY(VisibleAnywhere, Category = "Asset Config")
	FPrimaryAssetType assetType = DataAssetType::PSBlueprintAsset;

	void PostInitProperties() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};