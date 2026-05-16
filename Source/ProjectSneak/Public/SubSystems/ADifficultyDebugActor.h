// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UDifficultySubsystem.h"
#include "EventManagerSubsystem.h"
#include "Keys.h"
#include "Engine/World.h"
#include "ADifficultyDebugActor.generated.h"

UCLASS()
class PROJECTSNEAK_API ADifficultyDebugActor : public AActor
{
	GENERATED_BODY()


public:
	ADifficultyDebugActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	// Handles so we can unbind on EndPlay
	FDelegateHandle Handle_Disturb;
	FDelegateHandle Handle_Instab;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bPrintEveryTick = false;

	// Event callbacks (no need for UFUNCTION for native multicast)
	void OnDisturbanceEvent(const FParamBlackboard& BB);
	void OnInstabilityEvent(const FParamBlackboard& BB);

};
