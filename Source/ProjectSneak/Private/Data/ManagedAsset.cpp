// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/ManagedAsset.h"

//int64 GuidUtils::GenerateNew()
//{
//    int64 longResult = 0;
//
//    constexpr int guidLength = 64;
//    constexpr int64 zero = 0;
//    constexpr int64 one = 1;
//
//    for (int i = 0; i < guidLength; i++)
//    {
//        int64 mask = FMath::RandBool() ? (zero << i) : (one << i);
//        longResult = longResult | mask;
//    }
//
//    return longResult;
//}
//
//FString GuidUtils::ToString(int64 value)
//{
//    return LexToString(value);
//}
//
//bool GuidUtils::IsValid(int64 value)
//{
//    return value != 0;
//}


void UManagedDataAsset::PostInitProperties()
{
    Super::PostInitProperties();
    guid = FGuid::NewGuid();
}

FPrimaryAssetId UManagedDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(assetType, FName(*guid.ToString()));
}

void UManagedBlueprintAsset::PostInitProperties()
{
    Super::PostInitProperties();
    guid = FGuid::NewGuid();
}

FPrimaryAssetId UManagedBlueprintAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(assetType, FName(*guid.ToString()));
}