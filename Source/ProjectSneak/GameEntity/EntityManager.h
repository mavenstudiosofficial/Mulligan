// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameEntity.h"
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EntityManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSNEAK_API UEntityManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	TMap<FGuid, AGameEntity*> EntityMap;

	void RegisterEntity(AGameEntity* InGameEntity);
};
