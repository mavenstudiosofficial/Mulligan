// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "UProjectileDefinition.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EProjectileImpactPolicy : uint8
{
	DestroyOnHit,
	ExplodeOnHit,
	ExplodeOnTimer,
	Stick,
	Pierce
};


/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTSNEAK_API UProjectileDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Identity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FGameplayTag ProjectileTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	TSubclassOf<AActor> ProjectileActorClass;

	// Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float InitialSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float GravityScale = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bRotationFollowsVelocity = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lifetime")
	float LifeSeconds = 5.f;

	// Collision
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision")
	FName CollisionProfileName = "Projectile";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision")
	bool bCanHitOwner = false;

	// Impact
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	EProjectileImpactPolicy ImpactPolicy = EProjectileImpactPolicy::DestroyOnHit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact", meta = (ClampMin = "0.0"))
	float ExplodeDelaySeconds = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact", meta = (ClampMin = "0"))
	int32 MaxPierceCount = 0;

	// Payload (GAS)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Payload|SingleTarget")
	TArray<TSubclassOf<UGameplayEffect>> SingleTargetEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Payload|AoE", meta = (ClampMin = "0.0"))
	float AoERadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Payload|AoE")
	TArray<TSubclassOf<UGameplayEffect>> AoEEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Payload")
	bool bApplySingleTargetOnExplode = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Payload")
	bool bAffectsOwner = false;

	// Cues / Presentation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cues")
	FGameplayTag ImpactCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cues")
	FGameplayTag ExplodeCueTag;
};
