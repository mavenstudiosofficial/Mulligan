// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDifficultySubsystem.h"
#include "EventManagerSubsystem.h"
#include "Keys.h"
#include "Engine/World.h"
#include "ASampleController.generated.h"

UCLASS()
class PROJECTSNEAK_API ASampleController : public AActor
{
	GENERATED_BODY()


public:
	ASampleController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	void increaseCurrentVal();
private:
	int32 CurrentVal = 0;
};
