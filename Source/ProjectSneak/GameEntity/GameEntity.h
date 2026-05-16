// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameEntity.generated.h"

UCLASS()
class PROJECTSNEAK_API AGameEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "GameEntity")
	FGuid EntityGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameEntity")
	double TimeScale;
};
