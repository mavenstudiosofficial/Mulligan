// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PSAssetManager.h"

DEFINE_LOG_CATEGORY(LogPSAssetManager);

UPSAssetManager& UPSAssetManager::Get()
{
	UAssetManager& assetManager = UAssetManager::Get();
	return static_cast<UPSAssetManager&>(assetManager);
}

void UPSAssetManager::LoadAssetAsync(FGuid assetId, bool isBp, LoadedCallback callback)
{
	auto& assetManager = UPSAssetManager::Get();

	auto assetInfo = assetManager.m_assetInfo.Find(assetId);
	if (assetInfo && assetInfo->assetCache)
	{
		UE_LOG(LogPSAssetManager, Display, TEXT("Hit Cached!"));
		callback(assetId);
		return;
	}

	auto listener = assetManager.m_loadListener.Find(assetId);
	if (!listener)
	{
		FOnAssetLoaded loadedCallback;
		loadedCallback.AddLambda(callback);
		assetManager.m_loadListener.Emplace(assetId, loadedCallback);
	}
	else
	{
		listener->AddLambda(callback);
	}

	const auto& assetType = isBp ? DataAssetType::PSBlueprintAsset : DataAssetType::PSDataAsset;
	FPrimaryAssetId primaryId = FPrimaryAssetId(assetType, FName(*assetId.ToString()));
	TArray<FName> bundles;

	FStreamableDelegate delegate = FStreamableDelegate::CreateUObject(&assetManager, &UPSAssetManager::_OnAssetLoad, primaryId, assetId);
	assetManager.LoadPrimaryAsset(primaryId, bundles, delegate);
}

void UPSAssetManager::_OnAssetLoad(FPrimaryAssetId primaryId, FGuid guid)
{
	UE_LOG(LogPSAssetManager, Display, TEXT("Asset Loaded: %s"), *guid.ToString());

	auto& assetManager = UPSAssetManager::Get();

	auto assetPtr = assetManager.GetPrimaryAssetObject(primaryId);
	auto assetInfo = assetManager.m_assetInfo.Find(guid);

	if (assetInfo)
	{
		assetInfo->assetCache = assetPtr;
	}
	else
	{
		AssetInfo newInfo = AssetInfo();
		newInfo.assetCache = assetPtr;

		assetManager.m_assetInfo.Emplace(guid, newInfo);
	}

	auto listener = assetManager.m_loadListener.Find(guid);
	if (listener)
	{
		listener->Broadcast(guid);
		assetManager.m_loadListener.Remove(guid);
	}
}

void UPSAssetManager::IncreaseRefCount(const FGuid& guid)
{
	UE_LOG(LogPSAssetManager, Display, TEXT("Asset ref increase %s"), *guid.ToString());

	auto& assetManager = UPSAssetManager::Get();

	auto assetInfo = assetManager.m_assetInfo.Find(guid);
	if (assetInfo)
	{
		assetInfo->refCount++;
	}
}

void UPSAssetManager::DecreaseRefCount(const FGuid& guid)
{
	UE_LOG(LogPSAssetManager, Display, TEXT("Asset ref decrease %s"), *guid.ToString());

	auto& assetManager = UPSAssetManager::Get();

	auto assetInfo = assetManager.m_assetInfo.Find(guid);
	if (assetInfo)
	{
		assetInfo->refCount--;
	}
}
