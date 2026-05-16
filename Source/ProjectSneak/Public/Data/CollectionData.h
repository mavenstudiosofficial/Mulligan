// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/ManagedAsset.h"
#include "CollectionData.generated.h"

UINTERFACE(MinimalAPI)
class UCollectionData : public UInterface
{
	GENERATED_BODY()
};


class PROJECTSNEAK_API ICollectionData
{
	GENERATED_BODY()

public:
	virtual bool IsCollectionTypeBP();
	virtual UClass* GetCollectionType();
};


UCLASS()
class PROJECTSNEAK_API UManagedCollectionData : public UManagedDataAsset, public ICollectionData
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Collection Data")
	TSet<FGuid> collection;
};