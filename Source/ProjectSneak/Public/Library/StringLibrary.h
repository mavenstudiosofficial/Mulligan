// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSNEAK_API UStringLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String > String", CompactNodeTitle = ">", Keywords = "Compare String Greater"), Category = Game)
	static bool SGreaterThan(FString StringA, FString StringB);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String < String", CompactNodeTitle = "<", Keywords = "Compare String Less"), Category = Game)
	static bool SLessThan(FString StringA, FString StringB);
};
