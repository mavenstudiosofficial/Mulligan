// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityManager.h"

void UEntityManager::RegisterEntity(AGameEntity* InGameEntity) {
	if (!InGameEntity->EntityGuid.IsValid()) {
		InGameEntity->EntityGuid = FGuid::NewGuid();
	}
	while (!EntityMap.Contains(InGameEntity->EntityGuid)) {
		FGuid ConflictingId = InGameEntity->EntityGuid;
		InGameEntity->EntityGuid = FGuid::NewGuid();
		EntityMap.Add(InGameEntity->EntityGuid, InGameEntity);
		UE_LOG(LogTemp, Log, TEXT("Id Conflict: %s, New Id: %s"), *ConflictingId.ToString(), *(InGameEntity->EntityGuid).ToString());
	}
}