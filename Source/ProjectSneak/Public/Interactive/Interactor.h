// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactor.generated.h"

class APlayerController;
class IInteractive;

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class PROJECTSNEAK_API UInteractor : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractor();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual bool HasTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float interactDistance = 500.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	float focusDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* focusActor;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	APlayerController* playerController;
	AActor* playerActor;
	bool isTargetValid = false;
};
