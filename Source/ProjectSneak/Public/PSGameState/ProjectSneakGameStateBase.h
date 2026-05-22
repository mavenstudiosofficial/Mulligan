// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ProjectSneakGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRiskFloatChanged, float, NewValue);

/**
 * 
 */
UCLASS()
class PROJECTSNEAK_API AProjectSneakGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AProjectSneakGameStateBase();

	UPROPERTY(ReplicatedUsing = OnRep_Alert, BlueprintReadOnly, Category = "Risk")
	float Alert = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Disturbance, BlueprintReadOnly, Category = "Risk")
	float Disturbance = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Global Values")
	FOnRiskFloatChanged OnAlertChanged;

	UPROPERTY(BlueprintAssignable, Category = "Global Values")
	FOnRiskFloatChanged OnDisturbanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Global Values")
	FOnRiskFloatChanged OnDisturbanceBlockChanged;

	UPROPERTY(BlueprintAssignable, Category = "Global Values")
	FOnRiskFloatChanged OnNoiseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Global Values")
	FOnRiskFloatChanged OnInstabilityChanged;

	void AddAlert_ServerAuth(float AddAlert);
	void SetAlert_ServerAuth(float NewAlert);

	void AddDisturbance_ServerAuth(float AddDisturbance);
	void SetDisturbance_ServerAuth(float NewDisturbance);
	
	// =================
	// DISTURBANCE BLOCK
	// =================
	UPROPERTY(ReplicatedUsing = OnRep_DisturbanceBlock, BlueprintReadOnly, Category = "Risk")
	int32 DisturbanceBlock = 0;

	void AddDisturbanceBlock_ServerAuth(int32 DistrubanceBlock);
	void SetDisturbanceBlock_ServerAuth(int32 DistrubanceBlock);

	UFUNCTION(BlueprintCallable, Category = "Risk")
	void AddDisturbanceBlock(int32 AddDisturbanceBlock);

	UFUNCTION(Server, Reliable)
	void Server_UpdateDisturbanceBlock(int32 AddDisturbanceBlock);

	// =================
	// INSTABILITY
	// =================
	UPROPERTY(ReplicatedUsing = OnRep_Instability, BlueprintReadOnly, Category = "Risk")
	int32 Instability = 0;

	void AddInstability_ServerAuth(int32 AddInstability);
	void SetInstability_ServerAuth(int32 NewInstability);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void AddInstability(int32 AddInstability);

	UFUNCTION(Server, Reliable)
	void Server_UpdateInstability(int32 AddInstability);

	// ==========================
	// NOISE (manual replication)
	// ==========================

	// Server authoritative; NOT replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Risk")
	float Noise = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Risk|Noise")
	float NoiseMax = 1000.0f;

	// What clients should read (set via multicast).
	UPROPERTY(BlueprintReadOnly, Category = "Risk")
	float NoiseClient = 0.0f;

	// Flat decay rate when nobody has added noise since the last tick.
	// Units: Noise units per second.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Risk|Noise")
	float NoiseDecayPerSecond = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Risk|Noise", meta = (ClampMin = "0.0"))
	float NoiseDecayStartDelaySeconds = 0.0f;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetNoise(float NewNoise);

	void SetNoise_ServerAuth(float NewNoise);
	void AddNoise_ServerAuth(float NoiseDelta);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	static constexpr float NoiseBroadcastIntervalSeconds = 0.240f;

	FTimerHandle NoiseBroadcastTimerHandle;
	bool bNoiseAddedSinceLastBroadcast = false;
	float TimeSinceLastNoiseAddedSeconds = 0.0f;

	float ConsumeDisturbanceBlockForIncrease(float RequestedIncrease);

	void BroadcastNoiseToClients();

	UFUNCTION()
	void HandleNoiseChanged(float NewValue);

protected:
	UFUNCTION()
	void OnRep_Alert();

	UFUNCTION()
	void OnRep_Disturbance();

	UFUNCTION()
	void OnRep_DisturbanceBlock();

	UFUNCTION()
	void OnRep_Instability();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
