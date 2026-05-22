// Fill out your copyright notice in the Description page of Project Settings.

#include "UDifficultySubsystem.h"

#include "PSGameState/ProjectSneakGameStateBase.h"

bool UDifficultySubsystem::CanWriteAuthority(const TCHAR* Context) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDifficultySubsystem::%s failed: no valid world."), Context);
		return false;
	}

	if (World->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDifficultySubsystem::%s ignored on client world. Route to the server first via RPC, then call the subsystem on the server."), Context);
		return false;
	}

	return true;
}

AProjectSneakGameStateBase* UDifficultySubsystem::GetSneakGameState() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetGameState<AProjectSneakGameStateBase>();
	}

	return nullptr;
}

float UDifficultySubsystem::GetDisturbance() const
{
	if (const AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		return GS->Disturbance;
	}

	return 0.0f;
}

int32 UDifficultySubsystem::GetInstability() const
{
	if (const AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		return GS->Instability;
	}

	return 0;
}

float UDifficultySubsystem::GetAlert() const
{
	if (const AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		return GS->Alert;
	}

	return 0.0f;
}

int32 UDifficultySubsystem::GetDisturbanceBlock() const
{
	if (const AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		return static_cast<float>(GS->DisturbanceBlock);
	}

	return 0;
}

float UDifficultySubsystem::GetNoise() const
{
	if (const AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		return GS->HasAuthority() ? GS->Noise : GS->NoiseClient;
	}

	return 0.0f;
}

void UDifficultySubsystem::SetDisturbance(float NewDisturbance)
{
	if (!CanWriteAuthority(TEXT("SetDisturbance"))) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldDisturbance = GS->Disturbance;
		GS->SetDisturbance_ServerAuth(NewDisturbance);

		const float Delta = GS->Disturbance - OldDisturbance;
		if (!FMath::IsNearlyZero(Delta))
		{
			BroadcastDisturbanceChanged(GS->Disturbance, Delta);
		}
	}
}

void UDifficultySubsystem::AddDisturbance(float Delta)
{
	if (!CanWriteAuthority(TEXT("AddDisturbance"))) return;
	if (FMath::IsNearlyZero(Delta)) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldDisturbance = GS->Disturbance;
		const float OldBlock = static_cast<float>(GS->DisturbanceBlock);

		GS->AddDisturbance_ServerAuth(Delta);

		const float DisturbanceDelta = GS->Disturbance - OldDisturbance;
		const float BlockDelta = static_cast<float>(GS->DisturbanceBlock) - OldBlock;

		if (!FMath::IsNearlyZero(DisturbanceDelta))
		{
			BroadcastDisturbanceChanged(GS->Disturbance, DisturbanceDelta);
		}

		if (!FMath::IsNearlyZero(BlockDelta))
		{
			BroadcastDisturbanceBlockChanged(static_cast<float>(GS->DisturbanceBlock), BlockDelta);
		}
	}
}

void UDifficultySubsystem::SetInstability(int32 NewInstability)
{
	if (!CanWriteAuthority(TEXT("SetInstability"))) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const int32 OldInstability = GS->Instability;
		if (OldInstability == NewInstability) return;

		GS->SetInstability_ServerAuth(NewInstability);
		BroadcastInstabilityChanged(GS->Instability, GS->Instability - OldInstability);
	}
}

void UDifficultySubsystem::AddInstability(int32 Delta)
{
	if (!CanWriteAuthority(TEXT("AddInstability"))) return;
	if (Delta == 0) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const int32 OldInstability = GS->Instability;
		GS->AddInstability_ServerAuth(Delta);

		const int32 AppliedDelta = GS->Instability - OldInstability;
		if (AppliedDelta != 0)
		{
			BroadcastInstabilityChanged(GS->Instability, AppliedDelta);
		}
	}
}

void UDifficultySubsystem::SetAlert(float NewAlert)
{
	if (!CanWriteAuthority(TEXT("SetAlert"))) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldAlert = GS->Alert;
		const float Delta = NewAlert - OldAlert;
		if (FMath::IsNearlyZero(Delta)) return;

		GS->SetAlert_ServerAuth(NewAlert);
		BroadcastAlertChanged(GS->Alert, GS->Alert - OldAlert);
	}
}

void UDifficultySubsystem::AddAlert(float Delta)
{
	if (!CanWriteAuthority(TEXT("AddAlert"))) return;
	if (FMath::IsNearlyZero(Delta)) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldAlert = GS->Alert;
		GS->AddAlert_ServerAuth(Delta);

		const float AppliedDelta = GS->Alert - OldAlert;
		if (!FMath::IsNearlyZero(AppliedDelta))
		{
			BroadcastAlertChanged(GS->Alert, AppliedDelta);
		}
	}
}

void UDifficultySubsystem::SetDisturbanceBlock(int32 NewDisturbanceBlock)
{
	if (!CanWriteAuthority(TEXT("SetDisturbanceBlock"))) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const int32 OldBlock = GS->DisturbanceBlock;
		if (OldBlock == NewDisturbanceBlock) return;

		GS->SetDisturbanceBlock_ServerAuth(NewDisturbanceBlock);
		BroadcastDisturbanceBlockChanged(GS->DisturbanceBlock, GS->DisturbanceBlock - OldBlock);
	}
}

void UDifficultySubsystem::AddDisturbanceBlock(int32 Delta)
{
	if (!CanWriteAuthority(TEXT("AddDisturbanceBlock"))) return;
	if (Delta == 0) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const int32 OldBlock = GS->DisturbanceBlock;
		GS->AddDisturbanceBlock_ServerAuth(Delta);

		const int32 AppliedDelta = GS->DisturbanceBlock - OldBlock;
		if (AppliedDelta != 0)
		{
			BroadcastDisturbanceBlockChanged(GS->DisturbanceBlock, AppliedDelta);
		}
	}
}

void UDifficultySubsystem::SetNoise(float NewNoise)
{
	if (!CanWriteAuthority(TEXT("SetNoise"))) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldNoise = GS->Noise;
		const float OldAlert = GS->Alert;
		const float OldDisturbance = GS->Disturbance;

		GS->SetNoise_ServerAuth(NewNoise);

		const float NoiseDelta = GS->Noise - OldNoise;
		const float AlertDelta = GS->Alert - OldAlert;
		const float DisturbanceDelta = GS->Disturbance - OldDisturbance;

		if (!FMath::IsNearlyZero(NoiseDelta))
		{
			BroadcastNoiseChanged(GS->Noise, NoiseDelta);
		}

		if (!FMath::IsNearlyZero(AlertDelta))
		{
			BroadcastAlertChanged(GS->Alert, AlertDelta);
		}

		if (!FMath::IsNearlyZero(DisturbanceDelta))
		{
			BroadcastDisturbanceChanged(GS->Disturbance, DisturbanceDelta);
		}
	}
}

void UDifficultySubsystem::AddNoise(float Delta)
{
	if (!CanWriteAuthority(TEXT("AddNoise"))) return;
	if (Delta <= 0.0f) return;

	if (AProjectSneakGameStateBase* GS = GetSneakGameState())
	{
		const float OldNoise = GS->Noise;
		const float OldAlert = GS->Alert;
		const float OldDisturbance = GS->Disturbance;

		GS->AddNoise_ServerAuth(Delta);

		const float NoiseDelta = GS->Noise - OldNoise;
		const float AlertDelta = GS->Alert - OldAlert;
		const float DisturbanceDelta = GS->Disturbance - OldDisturbance;

		if (!FMath::IsNearlyZero(NoiseDelta))
		{
			BroadcastNoiseChanged(GS->Noise, NoiseDelta);
		}

		if (!FMath::IsNearlyZero(AlertDelta))
		{
			BroadcastAlertChanged(GS->Alert, AlertDelta);
		}

		if (!FMath::IsNearlyZero(DisturbanceDelta))
		{
			BroadcastDisturbanceChanged(GS->Disturbance, DisturbanceDelta);
		}
	}
}

void UDifficultySubsystem::BroadcastDisturbanceChanged(int32 Current, int32 Delta)
{
	OnDisturbanceChanged.Broadcast(Current, Delta);

	if (UWorld* World = GetWorld())
	{
		if (UEventManagerSubsystem* EMS = World->GetSubsystem<UEventManagerSubsystem>())
		{
			EMS->CallEvent(
				EventKeys::DISTURBANCE_CHANGED,
				BlackboardKeys::DISTURBANCE, Current,
				BlackboardKeys::VALUE_CHANGED, Delta
			);
		}
	}
}

void UDifficultySubsystem::BroadcastInstabilityChanged(int32 Current, int32 Delta)
{
	OnInstabilityChanged.Broadcast(Current, Delta);

	if (UWorld* World = GetWorld())
	{
		if (UEventManagerSubsystem* EMS = World->GetSubsystem<UEventManagerSubsystem>())
		{
			EMS->CallEvent(
				EventKeys::INSTABILITY_CHANGED,
				BlackboardKeys::INSTABILITY, Current,
				BlackboardKeys::VALUE_CHANGED, Delta
			);
		}
	}
}

void UDifficultySubsystem::BroadcastAlertChanged(float Current, float Delta)
{
	OnAlertChanged.Broadcast(Current, Delta);
}

void UDifficultySubsystem::BroadcastDisturbanceBlockChanged(int32 Current, int32 Delta)
{
	OnDisturbanceBlockChanged.Broadcast(Current, Delta);
}

void UDifficultySubsystem::BroadcastNoiseChanged(float Current, float Delta)
{
	OnNoiseChanged.Broadcast(Current, Delta);
}
