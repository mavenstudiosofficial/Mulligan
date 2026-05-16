// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"

#include "ManagedAsset.h"

#include "PSAssetManager.generated.h"

typedef TFunction<void(FGuid)> LoadedCallback;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAssetLoaded, FGuid);

PROJECTSNEAK_API DECLARE_LOG_CATEGORY_EXTERN(LogPSAssetManager, Log, All);

#define OnLoaded(Func) [this, weakThis = TWeakObjectPtr(this)](FGuid guid)\
{\
	if (weakThis.IsValid())\
	{\
		this->Func(guid);\
	}\
}

struct AssetInfo
{
public:
	int gcGeneration;
	int refCount;

	UObject* assetCache;
};

template<typename T>
struct AssetRef
{
public:
	AssetRef() = default;
	AssetRef(FGuid guid, T* asset);
	~AssetRef();

	void GrantRefTo(AssetRef<T>& other);
	void Clear();

	void operator=(const AssetRef<T>& other);

	T* assetPtr = nullptr;

private:
	FGuid m_assetGuid;
};



UCLASS()
class PROJECTSNEAK_API UPSAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UPSAssetManager& Get();
	
	template<typename T>
	static AssetRef<T> GetLoadedAsset(const FGuid& guid);

	static void LoadAssetAsync(FGuid assetId, bool isBp, LoadedCallback callback);

	static void IncreaseRefCount(const FGuid& guid);
	static void DecreaseRefCount(const FGuid& guid);

	static void CollectGarbage();
	
private:
	void _OnAssetLoad(FPrimaryAssetId assetId, FGuid guid);

	TMap<FGuid, FOnAssetLoaded> m_loadListener;
	TMap<FGuid, AssetInfo> m_assetInfo;
};

//template<typename T>
//AssetRef<T> UPSAssetManager::LoadAssetSync(const FGuid& assetId, bool isBp)
//{
//	auto& manager = UPSAssetManager::Get();
//	UObject* assetPtr;
//
//	auto assetInfo = manager.m_assetInfo.Find(guid);
//	if (assetInfo)
//	{
//		assetPtr = assetInfo.assetCache;
//	}
//	else
//	{
//		assetPtr = manager._LoadAssetAsync(assetId);
//	}
//
//	return AssetRef<T>(assetId, assetPtr);
//}


template<typename T>
AssetRef<T>::AssetRef(FGuid guid, T* asset)
{
	m_assetGuid = guid;
	assetPtr = asset;
	UPSAssetManager::IncreaseRefCount(guid);
}

template<typename T>
AssetRef<T>::~AssetRef()
{
	Clear();
}

template<typename T>
void AssetRef<T>::Clear()
{
	if (assetPtr)
	{
		assetPtr = nullptr;
		UPSAssetManager::DecreaseRefCount(m_assetGuid);
	}
}

template<typename T>
void AssetRef<T>::GrantRefTo(AssetRef<T>& other)
{
	if (assetPtr)
	{
		other.Clear();

		other.assetPtr = assetPtr;
		other.m_assetGuid = m_assetGuid;

		assetPtr = nullptr;
	}
}

template<typename T>
void AssetRef<T>::operator=(const AssetRef<T>& other)
{
	if (this == *other)
	{
		return;
	}

	Clear();
	m_assetGuid = other.m_assetGuid;
	assetPtr = other.assetPtr;

	UPSAssetManager::IncreaseRefCount(m_assetGuid);
}

template<typename T>
AssetRef<T> UPSAssetManager::GetLoadedAsset(const FGuid& guid)
{
	auto& manager = UPSAssetManager::Get();

	auto assetInfo = manager.m_assetInfo.Find(guid);
	if (assetInfo)
	{
		auto castedAsset = Cast<T>(assetInfo->assetCache);
		return AssetRef<T>(guid, castedAsset);
	}
	return AssetRef<T>();
}