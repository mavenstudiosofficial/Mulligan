#pragma once

#include "CoreMinimal.h"
#include "PSSessionsTypes.generated.h"

UENUM(BlueprintType)
enum class EPSessionFlowState : uint8
{
	Idle,
	Finding,
	Creating,
	Joining,
	Destroying,
	Traveling,
	InLobby,
	InMatch
};

UENUM(BlueprintType)
enum class EPSessionFlowError : uint8
{
	None,
	NoSubsystem,
	NoSessionInterface,
	Busy,
	InvalidRequest,
	CreateFailed,
	FindFailed,
	JoinFailed,
	JoinRejected,
	DestroyFailed,
	ResultNotFound,
	ResultStale,
	BuildMismatch,
	NoConnectString,
	TravelFailure,
	NetworkFailure
};

USTRUCT(BlueprintType)
struct FHostSessionRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	FString LobbyMapName = TEXT("Lobby");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	FString DisplayMapName = TEXT("Lobby");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	FString ModeName = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	FString SessionCode = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	int32 PublicConnections = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	int32 BuildId = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bAllowJoinInProgress = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bShouldAdvertise = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bAllowInvites = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bUsePresence = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bUseLobbiesIfAvailable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bIsLAN = false;
};

USTRUCT(BlueprintType)
struct FSessionSearchRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	int32 MaxResults = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	int32 RequiredBuildId = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bOnlySameBuild = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bOnlyJoinable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bOnlyLobbyState = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bUsePresence = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	bool bIsLAN = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sessions")
	float MaxEntryAgeSeconds = 10.0f;
};

USTRUCT(BlueprintType)
struct FSessionBrowserEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString StableId;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString SessionCode;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString HostName;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString MapName;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString ModeName;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString SessionState;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	int32 BuildId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	int32 PingMs = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	bool bIsJoinable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	bool bIsSameBuild = false;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	float FoundAtSeconds = 0.0f;
};