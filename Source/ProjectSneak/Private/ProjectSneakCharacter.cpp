// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectSneakCharacter.h"
#include "Projectile/ProjectSneakProjectile.h"
#include "ProjectileDefinition/ProjectileConfigurable.h"
#include "Animation/AnimInstance.h"
#include "Card/CardHolder.h"
#include "PSPlayerState/PSPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTagsManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystems/UDifficultySubsystem.h"
#include "ProjectSneak/Public/GAS/SharedAttributeSet.h"

//////////////////////////////////////////////////////////////////////////
// AProjectSneakCharacter

AProjectSneakCharacter::AProjectSneakCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//Add the ability system comp
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(GASRepMode);

	//Add the attribute set
	SharedAttributeSet = CreateDefaultSubobject<USharedAttributeSet>(TEXT("SharedAttributeSet"));
}

void AProjectSneakCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		BaseWalkSpeed = MoveComp->MaxWalkSpeed;
	}

	if (AbilitySystemComponent && HasAuthority()) {
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			DefaultAttributesEffect.GetDefaultObject(),
			1.0f,
			Context
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USharedAttributeSet::GetSpeedMultiplierAttribute())
			.AddUObject(this, &AProjectSneakCharacter::OnMoveSpeedChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.SpeedMultiplier")), EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AProjectSneakCharacter::OnSpeedMultiplierTagChanged);
	}
}

void AProjectSneakCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Noise generation while *actually moving* and running
	if (bRunning)
	{
		const float Speed2D = GetVelocity().Size2D();
		if (Speed2D > 5.0f) // small deadzone so standing still doesn't spam noise
		{
			// "Noise per second" => integrate over time
			AddNoise(RunNoisePerSecond * DeltaSeconds);
		}
	}
}

void AProjectSneakCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectSneakCharacter, bRunningRep);
	DOREPLIFETIME(AProjectSneakCharacter, ReplicatedLookPitch);
}

/// <summary>
/// Neck bend
/// </summary>

float AProjectSneakCharacter::MakeNeckBendPitch(float RawPitch) const
{
	return FMath::Clamp(-FRotator::NormalizeAxis(RawPitch), -60.f, 60.f);
}

void AProjectSneakCharacter::Server_SetLookPitch_Implementation(float NewPitch)
{
	ReplicatedLookPitch = MakeNeckBendPitch(NewPitch);
}

/// <summary>
/// Run Code
/// </summary>

float AProjectSneakCharacter::GetMaxWalkSpeed()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void AProjectSneakCharacter::RequestSwitchCharacter_Implementation(EPlayerClass character)
{
	this->SwitchCharacter(character);
}

void AProjectSneakCharacter::SwitchCharacter_Implementation(EPlayerClass character)
{
	characterClass = character;
	auto cardHolder = this->GetComponentByClass<UCardHolder>();
	if (cardHolder)
	{
		cardHolder->OnSwitchCharacter(character);
	}

	auto playerState = Cast<APSPlayerState>(this->GetPlayerState());
	if (playerState)
	{
		playerState->OnSwitchCharacter(character);
	}

	OnSwitchCharacter(character);
}

void AProjectSneakCharacter::Server_SetRunning_Implementation(bool bNewRunning)
{
	bRunningRep = bNewRunning;
	bRunning = bNewRunning;
	ApplyRunSpeed(bNewRunning);
}

void AProjectSneakCharacter::OnRep_Running()
{
	bRunning = bRunningRep;
	ApplyRunSpeed(bRunningRep);
}

void AProjectSneakCharacter::ApplyRunSpeed(bool bEnable)
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		float SpeedMultiplier = 1.0f;
		if (SharedAttributeSet)
		{
			SpeedMultiplier = SharedAttributeSet->SpeedMultiplier.GetCurrentValue();
		}
		if (BaseWalkSpeed <= 0.0f)
		{
			BaseWalkSpeed = MoveComp->MaxWalkSpeed;
		}

		MoveComp->MaxWalkSpeed = bEnable ? (BaseWalkSpeed * RunMultiplier * SpeedMultiplier) : (BaseWalkSpeed * SpeedMultiplier);
	}
}

void AProjectSneakCharacter::StartRun(const FInputActionValue&)
{
	if (!bMoveInputEnabled)
	{
		return;
	}

	// instant local feel
	bRunning = true;
	ApplyRunSpeed(true);

	// tell server (authority)
	if (!HasAuthority())
		Server_SetRunning(true);
	else
		bRunningRep = true;
}

void AProjectSneakCharacter::StopRun(const FInputActionValue&)
{
	bRunning = false;
	ApplyRunSpeed(false);

	if (!HasAuthority())
		Server_SetRunning(false);
	else
		bRunningRep = false;
}

/// <summary>
/// End
/// </summary>

void AProjectSneakCharacter::BindASCStateDelegates()
{
	if (!AbilitySystemComponent) return;

	// caches specific TAG if TAG exists
	if (!MovementBlockedTag.IsValid())
	{
		MovementBlockedTag = FGameplayTag::RequestGameplayTag(FName("State.Stun"));
	}

	//Add event for when TAG is added 
	AbilitySystemComponent
		->RegisterGameplayTagEvent(MovementBlockedTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AProjectSneakCharacter::OnMovementBlockedTagChanged);

	// call function once with current count to set initial state correctly
	OnMovementBlockedTagChanged(MovementBlockedTag, AbilitySystemComponent->GetTagCount(MovementBlockedTag));
}

void AProjectSneakCharacter::OnMovementBlockedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bBlocked = (NewCount > 0);

	SetCharacterControlEnabled(!bBlocked);

	if (bBlocked)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}
	}
}


//////////////////////////////////////////////////////////////////////////// Input

void AProjectSneakCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AProjectSneakCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AProjectSneakCharacter::StopJump);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectSneakCharacter::Move);

		//Running
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AProjectSneakCharacter::StartRun);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AProjectSneakCharacter::StopRun);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Canceled, this, &AProjectSneakCharacter::StopRun);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectSneakCharacter::Look);
	}
}

UAbilitySystemComponent* AProjectSneakCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AProjectSneakCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		BindASCStateDelegates();
	}
}

void AProjectSneakCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		BindASCStateDelegates();
	}
}

void AProjectSneakCharacter::Move(const FInputActionValue& Value)
{
	if (!bMoveInputEnabled || Controller == nullptr)
	{
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AProjectSneakCharacter::Look(const FInputActionValue& Value)
{
	if (!bLookInputEnabled || Controller == nullptr)
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
	AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity);

	const float NeckPitch = MakeNeckBendPitch(Controller->GetControlRotation().Pitch);

	if (HasAuthority())
	{
		ReplicatedLookPitch = NeckPitch;
	}
	else
	{
		Server_SetLookPitch(Controller->GetControlRotation().Pitch);
	}
}

void AProjectSneakCharacter::AddAlert(float NewAlert)
{
	if (HasAuthority())
	{
		if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
		{
			DifficultySubsystem->AddAlert(NewAlert);
		}
		return;
	}

	Server_UpdateAlert(NewAlert);
}

void AProjectSneakCharacter::AddDisturbance(float NewDisturbance)
{
	if (HasAuthority())
	{
		if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
		{
			DifficultySubsystem->AddDisturbance(NewDisturbance);
		}
		return;
	}

	Server_UpdateDisturbance(NewDisturbance);
}

void AProjectSneakCharacter::AddNoise(float AddNoise)
{
	if (AddNoise <= 0.0f) return;

	// Server applies immediately.
	if (HasAuthority())
	{
		if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
		{
			DifficultySubsystem->AddNoise(AddNoise);
		}
		return;
	}

	// Client: batch small deltas and send every 240ms.
	PendingNoiseDeltaToFlush += AddNoise;

	if (!GetWorldTimerManager().IsTimerActive(NoiseFlushTimerHandle))
	{
		GetWorldTimerManager().SetTimer(
			NoiseFlushTimerHandle,
			this,
			&AProjectSneakCharacter::FlushNoiseToServer,
			NoiseFlushIntervalSeconds,
			true
		);
	}
}

void AProjectSneakCharacter::Server_UpdateAlert_Implementation(float NewAlert)
{
	if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
	{
		DifficultySubsystem->AddAlert(NewAlert);
	}
}

void AProjectSneakCharacter::Server_UpdateDisturbance_Implementation(float NewDisturbance)
{
	if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
	{
		DifficultySubsystem->AddDisturbance(NewDisturbance);
	}
}

void AProjectSneakCharacter::Server_UpdateNoise_Implementation(float NewNoise)
{
	if (UDifficultySubsystem* DifficultySubsystem = GetWorld()->GetSubsystem<UDifficultySubsystem>())
	{
		DifficultySubsystem->AddNoise(NewNoise);
	}
}

void AProjectSneakCharacter::FireProjectilePressed()
{
	if (!EquippedProjectileDef) return;

	// Always route spawning to server
	if (HasAuthority())
	{
		SpawnProjectile_Server(EquippedProjectileDef);
	}
	else
	{
		Server_FireProjectile(EquippedProjectileDef);
	}
}

void AProjectSneakCharacter::Server_FireProjectile_Implementation(UProjectileDefinition* Definition)
{
	SpawnProjectile_Server(Definition);
}

void AProjectSneakCharacter::SpawnProjectile_Server(UProjectileDefinition* Definition)
{
	if (!HasAuthority() || !Definition || !Definition->ProjectileActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// Use control rotation so server matches what the player aimed
	const FRotator AimRot = Controller ? Controller->GetControlRotation() : GetActorRotation();

	const FVector SpawnLoc =
		FirstPersonCameraComponent
		? (FirstPersonCameraComponent->GetComponentLocation() + AimRot.RotateVector(MuzzleOffset))
		: (GetActorLocation() + AimRot.RotateVector(MuzzleOffset));

	FTransform SpawnTM(AimRot, SpawnLoc);

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// IMPORTANT: deferred spawn so we can set Definition before BeginPlay/replication
	AActor* Spawned = World->SpawnActorDeferred<AActor>(Definition->ProjectileActorClass, SpawnTM, this, this, Params.SpawnCollisionHandlingOverride);
	if (!Spawned) return;

	// Pass config to projectile (see Section 2 for the projectile-side API)
	if (Spawned->GetClass()->ImplementsInterface(UProjectileConfigurable::StaticClass()))
	{
		IProjectileConfigurable::Execute_InitializeFromDefinition(Spawned, Definition, this);
	}

	UGameplayStatics::FinishSpawningActor(Spawned, SpawnTM);
}


void AProjectSneakCharacter::SetGemCount(int32 value)
{
	GemCount = value;
}

void AProjectSneakCharacter::ChangeGemCount(int32 AdditionalCount)
{
	GemCount += AdditionalCount;
}

int32 AProjectSneakCharacter::GetGemCount() 
{
	return GemCount;
}

void AProjectSneakCharacter::SetCoinCount(int32 value)
{
	CoinCount = value;
}

void AProjectSneakCharacter::ChangeCoinCount(int32 AdditionalCount)
{
	CoinCount += AdditionalCount;
}

int32 AProjectSneakCharacter::GetCoinCount() 
{
	return CoinCount;
}

void AProjectSneakCharacter::ChangeKeyCount(int32 AdditionalCount)
{
	KeyCount += AdditionalCount;
}

int32 AProjectSneakCharacter::GetKeyCount()
{
	return KeyCount;
}

void AProjectSneakCharacter::SwapLifeState(APlayerController* player)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	FActorSpawnParameters SpawnParams{};
	SpawnParams.Owner = GetParentActor();
	SpawnParams.OverrideLevel = GetLevel();

	if (GameMode && OtherLifeState != nullptr) {
		auto Location = GetActorLocation();
		auto Rotation = GetActorRotation();

		AProjectSneakCharacter* NewLifeState = GetWorld()->SpawnActor<AProjectSneakCharacter>(
			OtherLifeState,
			Location,
			Rotation,
			SpawnParams
		);

		if (IsValid(NewLifeState)) {
			player->Possess(NewLifeState);
			NewLifeState->SetOwner(GetController());

			Destroy();
		}
	}
}


void AProjectSneakCharacter::FlushNoiseToServer()
{
	if (PendingNoiseDeltaToFlush <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(NoiseFlushTimerHandle);
		return;
	}

	const float DeltaToSend = PendingNoiseDeltaToFlush;
	PendingNoiseDeltaToFlush = 0.0f;

	// This is the single batched packet every 240ms.
	Server_UpdateNoise(DeltaToSend);
}

void AProjectSneakCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(NoiseFlushTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AProjectSneakCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data) 
{
	if (!GetCharacterMovement()) return;

	const float newMultiplier = Data.NewValue;

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * newMultiplier;
}

void AProjectSneakCharacter::OnSpeedMultiplierTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0) 
	{
		if (SharedAttributeSet)
		{
			const float NewMultiplier = SharedAttributeSet->SpeedMultiplier.GetCurrentValue();
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * NewMultiplier;
		}
	}
	else 
	{
		if (SharedAttributeSet)
		{
			SharedAttributeSet->SpeedMultiplier.SetCurrentValue(1.0f);
		}
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		}
	}
}

void AProjectSneakCharacter::SetMouseSensitivity(float Value)
{
	MouseSensitivity = Value * 0.16;
}

void AProjectSneakCharacter::SetMoveInputEnabled(bool bEnabled)
{
	bMoveInputEnabled = bEnabled;

	if (!bMoveInputEnabled)
	{
		bRunning = false;
		ApplyRunSpeed(false);

		if (HasAuthority())
		{
			bRunningRep = false;
		}
		else
		{
			Server_SetRunning(false);
		}
	}
}

void AProjectSneakCharacter::SetLookInputEnabled(bool bEnabled)
{
	bLookInputEnabled = bEnabled;
}

void AProjectSneakCharacter::SetCharacterControlEnabled(bool bEnabled)
{
	SetMoveInputEnabled(bEnabled);
	SetLookInputEnabled(bEnabled);
}

void AProjectSneakCharacter::SetMoveAndLookInputEnabled(bool bMoveEnabled, bool bLookEnabled)
{
	SetMoveInputEnabled(bMoveEnabled);
	SetLookInputEnabled(bLookEnabled);
}

void AProjectSneakCharacter::StartJump(const FInputActionValue&)
{
	if (!bMoveInputEnabled)
	{
		return;
	}

	Jump();
}

void AProjectSneakCharacter::StopJump(const FInputActionValue&)
{
	StopJumping();
}