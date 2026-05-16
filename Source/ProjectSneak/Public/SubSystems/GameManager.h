// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameReady);

UCLASS()
class PROJECTSNEAK_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool IsGameReady();
	
	void AddPendingSystem(USubsystem& system);
	void MakeSystemReady(USubsystem& system);

	UPROPERTY(BlueprintAssignable, Category = "GameManager")
	FOnGameReady onGameReady;
private:
	int m_pendingSystemAmount = 0;
};
