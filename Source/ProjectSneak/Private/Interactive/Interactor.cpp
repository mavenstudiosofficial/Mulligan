// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive/Interactor.h"
#include "Components/SphereComponent.h"

#include "Interactive/Interactive.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


UInteractor::UInteractor()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 10.0f;
}


void UInteractor::Interact()
{
	if (HasTarget())
	{
		IInteractive::Execute_OnInteract(focusActor, playerActor);
	}
}

bool UInteractor::HasTarget()
{
	return isTargetValid && focusDistance < interactDistance;
}

void UInteractor::BeginPlay()
{
	Super::BeginPlay();

	playerActor = GetOwner();
	APawn* pawn = Cast<APawn>(playerActor);

	if (pawn)
	{
		AController* controller = pawn->GetController();
		playerController = Cast<APlayerController>(controller);
	}
}

void UInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!playerController)
	{
		return;
	}

	FVector cameraPos;
	FRotator cameraRotation;
	playerController->GetPlayerViewPoint(cameraPos, cameraRotation);

	const FVector start = cameraPos;
	const FVector fwdVector = cameraRotation.Vector();
	const FVector end = start + (fwdVector * interactDistance);

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		hitResult,
		start,
		end,
		ECC_Visibility
	);

	focusActor = hitResult.GetActor();
	if (bHit && focusActor)
	{
		isTargetValid = focusActor->Implements<UInteractive>();

		const FVector focusActorPos = focusActor->GetActorLocation();
		const FVector currentPos = playerActor->GetActorLocation();

		focusDistance = (focusActorPos - currentPos).Length();
	}
	else
	{
		focusActor = nullptr;
		isTargetValid = false;
		focusDistance = 0;
	}
}

