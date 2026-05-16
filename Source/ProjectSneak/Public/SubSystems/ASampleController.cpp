// Fill out your copyright notice in the Description page of Project Settings.
#include "ASampleController.h"

ASampleController::ASampleController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASampleController::BeginPlay()
{
    Super::BeginPlay();
}

void ASampleController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void ASampleController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->WasInputKeyJustPressed(EKeys::Q))
    {
        increaseCurrentVal();
    }
}

void ASampleController::increaseCurrentVal()
{
    CurrentVal++;

    UE_LOG(LogTemp, Warning, TEXT("Q"));

    if (UDifficultySubsystem* system = GetWorld()->GetSubsystem<UDifficultySubsystem>())
    {
        system->SetDisturbance(CurrentVal);
        system->SetInstability(CurrentVal);
    }
}
