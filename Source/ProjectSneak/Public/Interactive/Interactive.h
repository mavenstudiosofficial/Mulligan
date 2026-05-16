// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UInteractive : public UInterface
{
	GENERATED_BODY()
};

class IInteractive
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(AActor* player);
};