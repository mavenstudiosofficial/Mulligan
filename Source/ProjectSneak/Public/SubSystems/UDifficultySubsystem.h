// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Keys.h"
#include "EventManagerSubsystem.h"
#include "UParamBlackboard.h"
#include "UDifficultySubSystem.generated.h"

class AProjectSneakGameStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDifficultyFloatChanged, float, Current, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDifficultyIntChanged, int32, Current, int32, Delta);

UCLASS()
class PROJECTSNEAK_API UDifficultySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override {}
	virtual void Deinitialize() override {}

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Disturbance")
	void SetDisturbance(float NewDisturbance);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Disturbance")
	void AddDisturbance(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Instability")
	void SetInstability(int32 NewInstability);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Instability")
	void AddInstability(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Alert")
	void SetAlert(float NewAlert);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Alert")
	void AddAlert(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|DisturbanceBlock")
	void SetDisturbanceBlock(int32 NewDisturbanceBlock);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|DisturbanceBlock")
	void AddDisturbanceBlock(int32 AddDisturbanceBlock);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Noise")
	void SetNoise(float NewNoise);

	UFUNCTION(BlueprintCallable, Category = "Difficulty|Noise")
	void AddNoise(float Delta);

	UPROPERTY(BlueprintAssignable, Category = "Difficulty")
	FOnDifficultyIntChanged OnDisturbanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Difficulty")
	FOnDifficultyIntChanged OnInstabilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Difficulty")
	FOnDifficultyFloatChanged OnAlertChanged;

	UPROPERTY(BlueprintAssignable, Category = "Difficulty")
	FOnDifficultyIntChanged OnDisturbanceBlockChanged;

	UPROPERTY(BlueprintAssignable, Category = "Difficulty")
	FOnDifficultyFloatChanged OnNoiseChanged;

	UFUNCTION(BlueprintPure, Category = "Difficulty|Disturbance")
	float GetDisturbance() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty|Instability")
	int32 GetInstability() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty|Alert")
	float GetAlert() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty|DisturbanceBlock")
	int32 GetDisturbanceBlock() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty|Noise")
	float GetNoise() const;

private:
	bool CanWriteAuthority(const TCHAR* Context) const;
	AProjectSneakGameStateBase* GetSneakGameState() const;

	void BroadcastDisturbanceChanged(int32 Current, int32 Delta);
	void BroadcastInstabilityChanged(int32 Current, int32 Delta);
	void BroadcastAlertChanged(float Current, float Delta);
	void BroadcastDisturbanceBlockChanged(int32 Current, int32 Delta);
	void BroadcastNoiseChanged(float Current, float Delta);
};
