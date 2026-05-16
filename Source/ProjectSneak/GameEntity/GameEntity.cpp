// Fill out your copyright notice in the Description page of Project Settings.


#include "GameEntity.h"
#include "EntityManager.h"

// Sets default values
AGameEntity::AGameEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EntityGuid = FGuid::NewGuid();
	TimeScale = 1.0;
}

// Called when the game starts or when spawned
void AGameEntity::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetSubsystem<UEntityManager>()) {
		GetWorld()->GetSubsystem<UEntityManager>()->RegisterEntity(this);
	}
}

// Called every frame
void AGameEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime * TimeScale);
}

