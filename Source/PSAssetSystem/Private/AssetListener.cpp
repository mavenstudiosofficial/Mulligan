// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetListener.h"

#include "Data/ManagedAsset.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

DEFINE_LOG_CATEGORY(LogPSAssetListener);

namespace
{
	FDelegateHandle s_addHandle;
	FDelegateHandle s_removeHandle;

	IAssetRegistry* s_registry = nullptr;

	const FString LISTENER_ROOT = TEXT("/Game");
	const FString WORLD_TYPE = TEXT("Engine.World");

	TMap<FName, UClass*> s_classCache;

	bool _FilterAsset(const FAssetData& assetData)
	{
		if (!assetData.IsValid() || !assetData.IsUAsset())
		{
			return false;
		}
		if (!assetData.GetObjectPathString().StartsWith(LISTENER_ROOT))
		{
			return false;
		}
		if (!assetData.GetPrimaryAssetId().IsValid())
		{
			return false;
		}

		return true;
	}

	UClass* _GetAssetClass(const FName& classPath)
	{
		auto result = s_classCache.Find(classPath);
		if (result)
		{
			return *result;
		}

		auto pathPtr = *classPath.ToString();
		UClass* loadedClass = Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, pathPtr));
		if (loadedClass)
		{
			s_classCache.Emplace(classPath, loadedClass);
			return loadedClass;
		}
		else
		{
			UE_LOG(LogPSAssetListener, Error, TEXT("Can't find StaticClass for %s"), pathPtr);
		}
	}
}

void UAssetListener::Startup()
{
	FAssetRegistryModule& module = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	s_registry = &module.Get();

	s_addHandle = s_registry->OnAssetAdded().AddStatic(&UAssetListener::_OnAssetAdded);
	s_removeHandle = s_registry->OnAssetRemoved().AddStatic(&UAssetListener::_OnAssetDeleted);
}

void UAssetListener::Shutdown()
{
	FAssetRegistryModule* module = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry");
	if (!module)
	{
		return;
	}

	s_registry = &module->Get();

	s_registry->OnAssetAdded().Remove(s_addHandle);
	s_registry->OnAssetRemoved().Remove(s_removeHandle);
}

void UAssetListener::_OnAssetAdded(const FAssetData& assetData)
{
	if (s_registry->IsLoadingAssets())
	{
		return;
	}
	if (!_FilterAsset(assetData))
	{
		return;
	}

}

void UAssetListener::_OnAssetDeleted(const FAssetData& assetData)
{
	if (s_registry->IsLoadingAssets())
	{
		return;
	}
	if (!_FilterAsset(assetData))
	{
		return;
	}

}