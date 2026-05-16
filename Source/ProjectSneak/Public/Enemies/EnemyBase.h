// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTSNEAK_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	int perceptionRange;
	int sightRange;

	int halfSightAngle;
	

	AEnemyBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
