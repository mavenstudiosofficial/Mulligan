// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectileConfigurable.generated.h"

class UProjectileDefinition;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileConfigurable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSNEAK_API IProjectileConfigurable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeFromDefinition(UProjectileDefinition* Definition, AActor* InInstigatorActor);
};
