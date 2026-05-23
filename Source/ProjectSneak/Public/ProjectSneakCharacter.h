// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ProjectileDefinition/UProjectileDefinition.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "PlayerSettingsTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectSneakCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UCardHolder;

UCLASS(config = Game)
class AProjectSneakCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Run Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PickupAction;


public:
	AProjectSneakCharacter();

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class USharedAttributeSet* SharedAttributeSet;

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	EGameplayEffectReplicationMode GASRepMode = EGameplayEffectReplicationMode::Mixed;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);

	void ApplyRunSpeed(bool bEnable);

	UPROPERTY(EditAnywhere, Category = "Movement|Run")
	float RunMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Stealth|Noise")
	float RunNoisePerSecond = 250.0f;  // tweak

	// Cached so we can restore exact walk speed
	float BaseWalkSpeed = 0.0f;

	bool bRunning = false;

	UPROPERTY(ReplicatedUsing = OnRep_Running)
	bool bRunningRep = false;

	UFUNCTION()
	void OnRep_Running();

	UFUNCTION(Server, Reliable)
	void Server_SetRunning(bool bNewRunning);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable)
	void Server_SetLookPitch(float NewPitch);

protected:
	FGameplayTag MovementBlockedTag;

	void BindASCStateDelegates();
	void OnMovementBlockedTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Global")
	void AddAlert(float NewAlert);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void AddDisturbance(float NewDisturbance);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void AddNoise(float AddNoise);

	UFUNCTION(Server, Reliable)
	void Server_UpdateAlert(float NewAlert);

	UFUNCTION(Server, Reliable)
	void Server_UpdateDisturbance(float NewDisturbance);

	UFUNCTION(Server, Reliable)
	void Server_UpdateNoise(float NewNoise);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileDefinition> EquippedProjectileDef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles", meta = (AllowPrivateAccess = "true"))
	FVector MuzzleOffset = FVector(100.f, 0.f, 0.f);

	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	void FireProjectilePressed();

	UFUNCTION(Server, Reliable)
	void Server_FireProjectile(UProjectileDefinition* Definition);

	void SpawnProjectile_Server(UProjectileDefinition* Definition);

	//NeckBend
	UPROPERTY(Replicated, BlueprintReadOnly)
	float ReplicatedLookPitch = 0.f;

	float MakeNeckBendPitch(float RawPitch) const;

private:

	static constexpr float NoiseFlushIntervalSeconds = 0.240f;
	float PendingNoiseDeltaToFlush = 0.0f;
	FTimerHandle NoiseFlushTimerHandle;

	void FlushNoiseToServer();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(ReplicatedUsing = OnRep_GemCount, BlueprintReadOnly, Category = "Currency", meta = (AllowPrivateAccess = "true"))
	int32 GemCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CoinCount, BlueprintReadOnly, Category = "Currency", meta = (AllowPrivateAccess = "true"))
	int32 CoinCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_KeyCount, BlueprintReadOnly, Category = "Currency", meta = (AllowPrivateAccess = "true"))
	int32 KeyCount = 0;

	UFUNCTION()
	void OnRep_GemCount();

	UFUNCTION()
	void OnRep_CoinCount();

	UFUNCTION()
	void OnRep_KeyCount();

	UFUNCTION(Server, Reliable)
	void Server_SetGemCount(int32 Value);

	UFUNCTION(Server, Reliable)
	void Server_ChangeGemCount(int32 AdditionalCount);

	UFUNCTION(Server, Reliable)
	void Server_SetCoinCount(int32 Value);

	UFUNCTION(Server, Reliable)
	void Server_ChangeCoinCount(int32 AdditionalCount);

	UFUNCTION(Server, Reliable)
	void Server_ChangeKeyCount(int32 AdditionalCount);

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TSubclassOf<class AProjectSneakCharacter> OtherLifeState;

public:

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void SetGemCount(int32 value);
	UFUNCTION(BlueprintCallable, Category = "Currency")
	void ChangeGemCount(int32 AdditionalCount);
	UFUNCTION(BlueprintCallable, Category = "Currency")
	int32 GetGemCount();

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void SetCoinCount(int32 value);
	UFUNCTION(BlueprintCallable, Category = "Currency")
	void ChangeCoinCount(int32 AdditionalCount);
	UFUNCTION(BlueprintCallable, Category = "Currency")
	int32 GetCoinCount();

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void ChangeKeyCount(int32 AdditionalCount);
	UFUNCTION(BlueprintCallable, Category = "Currency")
	int32 GetKeyCount();

	UFUNCTION(BlueprintCallable)
	void SwapLifeState(APlayerController* player);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

private:
	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);

	void OnSpeedMultiplierTagChanged(const FGameplayTag Tag, int32 NewCount);

public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	float GetMaxWalkSpeed();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	EPlayerClass characterClass;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestSwitchCharacter(EPlayerClass character);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character")
	void SwitchCharacter(EPlayerClass character);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void OnSwitchCharacter(EPlayerClass character);

protected:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0", UIMax = "5.0"), Category = Input)
	float MouseSensitivity = 1.0;

public:
	UFUNCTION(BlueprintCallable, Category = Input)
	void SetMouseSensitivity(float Value);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Input|Control")
	bool bMoveInputEnabled = true;

	UPROPERTY(BlueprintReadOnly, Category = "Input|Control")
	bool bLookInputEnabled = true;

public:
	UFUNCTION(BlueprintCallable, Category = "Input|Control")
	void SetMoveInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Input|Control")
	void SetLookInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Input|Control")
	void SetCharacterControlEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Input|Control")
	void SetMoveAndLookInputEnabled(bool bMoveEnabled, bool bLookEnabled);

	UFUNCTION(BlueprintPure, Category = "Input|Control")
	bool IsMoveInputEnabled() const { return bMoveInputEnabled; }

	UFUNCTION(BlueprintPure, Category = "Input|Control")
	bool IsLookInputEnabled() const { return bLookInputEnabled; }
};

