// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileDefinition/ProjectileConfigurable.h"
#include "ProjectSneakProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UProjectileDefinition;

UCLASS(config=Game)
class AProjectSneakProjectile : public AActor, public IProjectileConfigurable
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AProjectSneakProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void InitializeFromDefinition_Implementation(UProjectileDefinition* Definition, AActor* InInstigatorActor) override;
	
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Definition)
	TObjectPtr<UProjectileDefinition> DefinitionAsset;

	UFUNCTION()
	void OnRep_Definition();

	void ApplyDefinition();

	// --- Impact handling helpers ---
	void HandleImpact(AActor* OtherActor, const FHitResult& Hit);
	void ExplodeAt(const FVector& Epicenter);

	void ApplySingleTargetEffects(AActor* TargetActor);
	void ApplyAoEEffects(const FVector& Epicenter);

	// Simple state
	bool bHasImpacted = false;
	int32 PierceHitCount = 0;

	FTimerHandle ExplodeTimerHandle;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

