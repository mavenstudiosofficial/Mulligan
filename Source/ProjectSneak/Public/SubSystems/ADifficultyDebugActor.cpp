// Fill out your copyright notice in the Description page of Project Settings.
#include "ADifficultyDebugActor.h"

ADifficultyDebugActor::ADifficultyDebugActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ADifficultyDebugActor::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        if (UEventManagerSubsystem* EMS = World->GetSubsystem<UEventManagerSubsystem>())
        {
            Handle_Disturb = EMS->AddListener(EventKeys::DISTURBANCE_CHANGED, this, &ADifficultyDebugActor::OnDisturbanceEvent);
            Handle_Instab  = EMS->AddListener(EventKeys::INSTABILITY_CHANGED,  this, &ADifficultyDebugActor::OnInstabilityEvent);
        }
    }
}

void ADifficultyDebugActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UEventManagerSubsystem* EMS = World->GetSubsystem<UEventManagerSubsystem>())
        {
            EMS->RemoveListener(EventKeys::DISTURBANCE_CHANGED, Handle_Disturb);
            EMS->RemoveListener(EventKeys::INSTABILITY_CHANGED,  Handle_Instab);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void ADifficultyDebugActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADifficultyDebugActor::OnDisturbanceEvent(const FParamBlackboard& BB)
{
    /*int32 Current = 0, Delta = 0;
    BB.TryGetParam<int32>(BlackboardKeys::DISTURBANCE,   Current);
    BB.TryGetParam<int32>(BlackboardKeys::VALUE_CHANGED, Delta);

    UE_LOG(LogTemp, Warning, TEXT("[Debug] Disturbance = %d (Δ %d)"), Current, Delta);*/
}

void ADifficultyDebugActor::OnInstabilityEvent(const FParamBlackboard& BB)
{
    /*int32 Current = 0, Delta = 0;
    BB.TryGetParam<int32>(BlackboardKeys::INSTABILITY,   Current);
    BB.TryGetParam<int32>(BlackboardKeys::VALUE_CHANGED, Delta);

    UE_LOG(LogTemp, Warning, TEXT("[Debug] Instability = %d (Δ %d)"), Current, Delta);*/
}


/*
// Sets default values
ADifficultyDebugActor::ADifficultyDebugActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADifficultyDebugActor::BeginPlay()
{
	Super::BeginPlay();

	// Fetch the world subsystem (works in game world; not available in editor-only worlds)
	if (UWorld* World = GetWorld())
	{
		DifficultySubsystem = World->GetSubsystem<UDifficultySubsystem>();
	}

	if (!DifficultySubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("DifficultyDebugActor: UDifficultySubsystem not found on this world"));
		return;
	}

	// Bind to change events (delegates are Dynamic, so the handler must be a UFUNCTION on a UObject)
	DifficultySubsystem->OnDisturbanceChanged.AddDynamic(this, &ADifficultyDebugActor::HandleDisturbanceChanged);
	DifficultySubsystem->OnInstabilityChanged.AddDynamic(this, &ADifficultyDebugActor::HandleInstabilityChanged);

	// Example: read current values once on BeginPlay
	const int32 CurrentDisturbance = DifficultySubsystem->GetDisturbance();
	const int32 CurrentInstability = DifficultySubsystem->GetInstability();

	UE_LOG(LogTemp, Log, TEXT("BeginPlay: Disturbance=%d, Instability=%d"), CurrentDisturbance, CurrentInstability);
}

void ADifficultyDebugActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPrintEveryTick && DifficultySubsystem)
	{
		UE_LOG(LogTemp, VeryVerbose, TEXT("Tick: Disturbance=%d, Instability=%d"),
			DifficultySubsystem->GetDisturbance(), DifficultySubsystem->GetInstability());
	}

	UDifficultySubsystem* system = GetWorld()->GetSubsystem<UDifficultySubsystem>();

}

void ADifficultyDebugActor::HandleDisturbanceChanged(int32 Current, int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("Disturbance changed: Current=%d (Delta=%+d)"), Current, Delta);
	// You can drive UI, VFX, or gameplay from here.
}

void ADifficultyDebugActor::HandleInstabilityChanged(int32 Current, int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("Instability changed: Current=%d (Delta=%+d)"), Current, Delta);
	// Drive other systems here.
}
 */

