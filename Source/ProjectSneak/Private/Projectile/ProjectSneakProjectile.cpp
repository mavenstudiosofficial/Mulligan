// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectile/ProjectSneakProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "ProjectileDefinition/UProjectileDefinition.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Kismet/KismetSystemLibrary.h"

AProjectSneakProjectile::AProjectSneakProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectSneakProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AProjectSneakProjectile::BeginPlay() {
	Super::BeginPlay();

	ApplyDefinition();

	if (HasAuthority() && DefinitionAsset)
	{
		if (DefinitionAsset->ImpactPolicy == EProjectileImpactPolicy::ExplodeOnTimer &&
			DefinitionAsset->ExplodeDelaySeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				ExplodeTimerHandle,
				[this]()
				{
					if (!bHasImpacted) // if you want timer to explode even without impact, remove this guard
					{
						ExplodeAt(GetActorLocation());
					}
				},
				DefinitionAsset->ExplodeDelaySeconds,
				false
			);
		}
	}
}

void AProjectSneakProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}

void AProjectSneakProjectile::InitializeFromDefinition_Implementation(UProjectileDefinition* InDefinition, AActor* InInstigatorActor)
{
	if (!HasAuthority())
	{
		return; // server sets replicated config
	}

	DefinitionAsset = InDefinition;

	// Apply immediately on server before BeginPlay
	ApplyDefinition();

	// Set initial velocity based on spawn rotation
	if (ProjectileMovement && DefinitionAsset)
	{
		const FVector Dir = GetActorForwardVector();
		ProjectileMovement->Velocity = Dir * DefinitionAsset->InitialSpeed;
	}
}

void AProjectSneakProjectile::OnRep_Definition()
{
	ApplyDefinition();
}

void AProjectSneakProjectile::ApplyDefinition()
{
	if (!DefinitionAsset)
	{
		return;
	}

	// Collision profile
	if (CollisionComp)
	{
		CollisionComp->SetCollisionProfileName(DefinitionAsset->CollisionProfileName);

		if (!DefinitionAsset->bCanHitOwner && GetOwner())
		{
			CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
		}
	}

	// Lifetime
	SetLifeSpan(DefinitionAsset->LifeSeconds);

	// Movement tuning
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = DefinitionAsset->InitialSpeed;
		ProjectileMovement->MaxSpeed = DefinitionAsset->MaxSpeed;
		ProjectileMovement->ProjectileGravityScale = DefinitionAsset->GravityScale;
		ProjectileMovement->bRotationFollowsVelocity = DefinitionAsset->bRotationFollowsVelocity;

		// Optional: bounce behavior could be added to definition later
		// ProjectileMovement->bShouldBounce = ...
	}
}

void AProjectSneakProjectile::HandleImpact(AActor* OtherActor, const FHitResult& Hit)
{
	if (!DefinitionAsset)
	{
		Destroy();
		return;
	}

	// Mark impacted for most policies (pierce is special)
	const EProjectileImpactPolicy Policy = DefinitionAsset->ImpactPolicy;

	switch (Policy)
	{
	case EProjectileImpactPolicy::DestroyOnHit:
		bHasImpacted = true;
		ApplySingleTargetEffects(OtherActor);
		Destroy();
		break;

	case EProjectileImpactPolicy::ExplodeOnHit:
		bHasImpacted = true;
		ExplodeAt(Hit.ImpactPoint);
		break;

	case EProjectileImpactPolicy::ExplodeOnTimer:
		// If you want “impact triggers timer”, start timer here if not started.
		// For now: allow projectile to continue until timer fires.
		break;

	case EProjectileImpactPolicy::Stick:
		bHasImpacted = true;
		if (ProjectileMovement)
		{
			ProjectileMovement->StopMovementImmediately();
			ProjectileMovement->Deactivate();
		}
		// Minimal stick: freeze in place. If you want attach-to-surface, implement AttachToComponent using Hit.
		if (DefinitionAsset->ExplodeDelaySeconds > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				ExplodeTimerHandle,
				[this]() { ExplodeAt(GetActorLocation()); },
				DefinitionAsset->ExplodeDelaySeconds,
				false
			);
		}
		else
		{
			// No delay: treat like DestroyOnHit
			ApplySingleTargetEffects(OtherActor);
			Destroy();
		}
		break;

	case EProjectileImpactPolicy::Pierce:
		ApplySingleTargetEffects(OtherActor);
		PierceHitCount++;

		if (DefinitionAsset->MaxPierceCount > 0 && PierceHitCount >= DefinitionAsset->MaxPierceCount)
		{
			bHasImpacted = true;
			Destroy();
		}
		break;

	default:
		bHasImpacted = true;
		Destroy();
		break;
	}
}

void AProjectSneakProjectile::ExplodeAt(const FVector& Epicenter)
{
	if (!DefinitionAsset)
	{
		Destroy();
		return;
	}

	// Optionally apply direct-hit effects too (rocket behavior)
	// For your freeze bolt, you’ll typically use DestroyOnHit and AoERadius=0.
	ApplyAoEEffects(Epicenter);

	Destroy();
}

void AProjectSneakProjectile::ApplySingleTargetEffects(AActor* TargetActor)
{
	if (!DefinitionAsset || !TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect>& GEClass : DefinitionAsset->SingleTargetEffects)
	{
		if (!GEClass) continue;

		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(GEClass, 1.0f, Ctx);
		if (Spec.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	}
}


void AProjectSneakProjectile::ApplyAoEEffects(const FVector& Epicenter)
{
	if (!DefinitionAsset)
	{
		return;
	}

	if (DefinitionAsset->AoERadius <= 0.f || DefinitionAsset->AoEEffects.Num() == 0)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!SourceASC)
	{
		return;
	}

	TArray<AActor*> Ignored;
	if (!DefinitionAsset->bAffectsOwner && GetOwner())
	{
		Ignored.Add(GetOwner());
	}

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Epicenter,
		DefinitionAsset->AoERadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>(), // leave empty to overlap all, or constrain later
		AActor::StaticClass(),
		Ignored,
		OverlappedActors
	);

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	for (AActor* A : OverlappedActors)
	{
		if (!A) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(A);
		if (!TargetASC) continue;

		for (const TSubclassOf<UGameplayEffect>& GEClass : DefinitionAsset->AoEEffects)
		{
			if (!GEClass) continue;

			FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(GEClass, 1.0f, Ctx);
			if (Spec.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
			}
		}
	}
}

void AProjectSneakProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectSneakProjectile, DefinitionAsset);
}