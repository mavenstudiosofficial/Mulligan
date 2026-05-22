// Fill out your copyright notice in the Description page of Project Settings.


#include "PSGameState/ProjectSneakGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "TimerManager.h"

AProjectSneakGameStateBase::AProjectSneakGameStateBase()
{
	bReplicates = true;
}

void AProjectSneakGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectSneakGameStateBase, Alert);
	DOREPLIFETIME(AProjectSneakGameStateBase, Disturbance);
	DOREPLIFETIME(AProjectSneakGameStateBase, DisturbanceBlock);
	DOREPLIFETIME(AProjectSneakGameStateBase, Instability);
}

void AProjectSneakGameStateBase::OnRep_Alert()
{
	OnAlertChanged.Broadcast(Alert);
}

void AProjectSneakGameStateBase::OnRep_Disturbance()
{
	OnDisturbanceChanged.Broadcast(Disturbance);
}

void AProjectSneakGameStateBase::OnRep_DisturbanceBlock()
{
	OnDisturbanceBlockChanged.Broadcast(DisturbanceBlock);
}

void AProjectSneakGameStateBase::OnRep_Instability()
{
	OnInstabilityChanged.Broadcast(Instability);
}

void AProjectSneakGameStateBase::SetAlert_ServerAuth(float NewAlert)
{
	if (!HasAuthority()) return;

	Alert = NewAlert;
	OnRep_Alert();    
	ForceNetUpdate(); 
}

void AProjectSneakGameStateBase::AddAlert_ServerAuth(float AddAlert)
{
	if (!HasAuthority()) return;

	Alert += AddAlert;
	OnRep_Alert();
	ForceNetUpdate();
}

void AProjectSneakGameStateBase::SetNoise_ServerAuth(float NewNoise)
{
	if (!HasAuthority()) return;

	if (NoiseMax > 0.0f && NewNoise >= NoiseMax)
	{
		const int32 ThresholdsCrossed = FMath::Max(1, FMath::FloorToInt(NewNoise / NoiseMax));

		Alert += static_cast<float>(ThresholdsCrossed);

		const float DisturbanceIncrease = ConsumeDisturbanceBlockForIncrease(static_cast<float>(ThresholdsCrossed));
		if (DisturbanceIncrease > 0.0f)
		{
			Disturbance += DisturbanceIncrease;
			OnRep_Disturbance();
		}

		Noise = 0.0f;

		OnRep_Alert();
		ForceNetUpdate();
	}
	else
	{
		Noise = FMath::Clamp(NewNoise, 0.0f, NoiseMax);
	}

	bNoiseAddedSinceLastBroadcast = true;
	TimeSinceLastNoiseAddedSeconds = 0.0f;
}

void AProjectSneakGameStateBase::AddNoise_ServerAuth(float NoiseDelta)
{
	if (!HasAuthority() || NoiseDelta <= 0.0f) return;

	if (NoiseMax > 0.0f)
	{
		const float NewTotal = Noise + NoiseDelta;

		if (NewTotal >= NoiseMax)
		{
			const int32 ThresholdsCrossed = FMath::Max(1, FMath::FloorToInt(NewTotal / NoiseMax));

			Alert += static_cast<float>(ThresholdsCrossed);

			const float DisturbanceIncrease = ConsumeDisturbanceBlockForIncrease(static_cast<float>(ThresholdsCrossed));
			if (DisturbanceIncrease > 0.0f)
			{
				Disturbance += DisturbanceIncrease;
				OnRep_Disturbance();
			}

			Noise = 0.0f;

			OnRep_Alert();
			ForceNetUpdate();
		}
		else
		{
			Noise = NewTotal;
		}
	}
	else
	{
		Noise += NoiseDelta;
	}

	bNoiseAddedSinceLastBroadcast = true;
	TimeSinceLastNoiseAddedSeconds = 0.0f;
}

void AProjectSneakGameStateBase::SetDisturbance_ServerAuth(float NewDisturbance)
{
	if (!HasAuthority()) return;
	if (NewDisturbance < 0.0f) return;

	const float RequestedIncrease = NewDisturbance - Disturbance;

	if (RequestedIncrease <= 0.0f)
	{
		Disturbance = NewDisturbance;
		OnRep_Disturbance();
		ForceNetUpdate();
		return;
	}

	const float AppliedIncrease = ConsumeDisturbanceBlockForIncrease(RequestedIncrease);

	if (AppliedIncrease > 0.0f)
	{
		Disturbance += AppliedIncrease;
		OnRep_Disturbance();
	}

	ForceNetUpdate();
}

void AProjectSneakGameStateBase::AddDisturbance_ServerAuth(float AddDisturbance)
{
	if (!HasAuthority()) return;
	if (AddDisturbance <= 0.0f) return;

	const float AppliedIncrease = ConsumeDisturbanceBlockForIncrease(AddDisturbance);

	if (AppliedIncrease > 0.0f)
	{
		Disturbance += AppliedIncrease;
		OnRep_Disturbance();
	}

	ForceNetUpdate();
}

void AProjectSneakGameStateBase::SetInstability_ServerAuth(int32 NewInstability)
{
	if (!HasAuthority()) return;

	Instability = NewInstability;
	OnRep_Instability();
	ForceNetUpdate();
}

void AProjectSneakGameStateBase::AddInstability_ServerAuth(int32 AddInstability)
{
	if (!HasAuthority()) return;

	Instability += AddInstability;

	OnRep_Instability();
	ForceNetUpdate();
}

void AProjectSneakGameStateBase::SetDisturbanceBlock_ServerAuth(int32 NewDisturbanceBlock)
{
	if (!HasAuthority()) return;

	DisturbanceBlock = FMath::Max(0, NewDisturbanceBlock);
	OnRep_DisturbanceBlock();
	ForceNetUpdate();
}

void AProjectSneakGameStateBase::AddDisturbanceBlock_ServerAuth(int32 AddDisturbanceBlock)
{
	if (!HasAuthority()) return;

	DisturbanceBlock = FMath::Max(0, DisturbanceBlock + AddDisturbanceBlock);

	OnRep_DisturbanceBlock();
	ForceNetUpdate();
}

void AProjectSneakGameStateBase::AddDisturbanceBlock(int32 AddDisturbanceBlock)
{
	if (HasAuthority())
	{
		AddDisturbanceBlock_ServerAuth(AddDisturbanceBlock);
	}
	else 
	{
		Server_UpdateDisturbanceBlock(AddDisturbanceBlock);
	}
}

void AProjectSneakGameStateBase::AddInstability(int32 AddInstability)
{
	if (HasAuthority())
	{
		AddInstability_ServerAuth(AddInstability);
	}
	else
	{
		Server_UpdateInstability(AddInstability);
	}
}

void AProjectSneakGameStateBase::Server_UpdateInstability_Implementation(int32 AddInstability)
{
	if (HasAuthority())
	{
		AddInstability_ServerAuth(AddInstability);
	}
}

void AProjectSneakGameStateBase::Server_UpdateDisturbanceBlock_Implementation(int32 AddDisturbanceBlock)
{
	if (HasAuthority())
	{
		AddDisturbanceBlock_ServerAuth(AddDisturbanceBlock);
	}
}

void AProjectSneakGameStateBase::HandleNoiseChanged(float NewValue)
{
	UE_LOG(LogTemp, Verbose, TEXT("Noise updated to: %f"), NewValue);
	OnNoiseChanged.Broadcast(NewValue);
}

void AProjectSneakGameStateBase::Multicast_SetNoise_Implementation(float NewNoise)
{
	if (HasAuthority())
	{
		return;
	}

	NoiseClient = FMath::Clamp(NewNoise, 0.0f, NoiseMax);
	HandleNoiseChanged(NoiseClient);
}

void AProjectSneakGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	NoiseClient = Noise;
	HandleNoiseChanged(NoiseClient);

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			NoiseBroadcastTimerHandle,
			this,
			&AProjectSneakGameStateBase::BroadcastNoiseToClients,
			NoiseBroadcastIntervalSeconds,
			true
		);
	}
}

void AProjectSneakGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(NoiseBroadcastTimerHandle);
	Super::EndPlay(EndPlayReason);
}

float AProjectSneakGameStateBase::ConsumeDisturbanceBlockForIncrease(float RequestedIncrease)
{
	if (RequestedIncrease <= 0.0f)
	{
		return 0.0f;
	}

	if (DisturbanceBlock <= 0)
	{
		return RequestedIncrease;
	}

	const int32 OldBlock = DisturbanceBlock;
	const int32 BlockCost = FMath::CeilToInt(RequestedIncrease);
	const int32 BlockSpent = FMath::Min(DisturbanceBlock, BlockCost);

	DisturbanceBlock = FMath::Max(0, DisturbanceBlock - BlockSpent);

	if (DisturbanceBlock != OldBlock)
	{
		OnRep_DisturbanceBlock();
	}

	return FMath::Max(0.0f, RequestedIncrease - static_cast<float>(BlockSpent));
}

void AProjectSneakGameStateBase::BroadcastNoiseToClients()
{
	if (!HasAuthority()) return;

	if (bNoiseAddedSinceLastBroadcast)
	{
		TimeSinceLastNoiseAddedSeconds = 0.0f;
	}
	else
	{
		TimeSinceLastNoiseAddedSeconds += NoiseBroadcastIntervalSeconds;

		const float EffectiveDelay = FMath::Max(0.0f, NoiseDecayStartDelaySeconds);
		if (TimeSinceLastNoiseAddedSeconds >= EffectiveDelay && NoiseDecayPerSecond > 0.0f)
		{
			const float Decay = NoiseDecayPerSecond * NoiseBroadcastIntervalSeconds;
			Noise = FMath::Max(0.0f, Noise - Decay);
		}
	}

	bNoiseAddedSinceLastBroadcast = false;

	// Update listen-server UI immediately.
	NoiseClient = Noise;
	HandleNoiseChanged(NoiseClient);

	// Manual replication: multicast authoritative noise to clients.
	Multicast_SetNoise(Noise);
}
