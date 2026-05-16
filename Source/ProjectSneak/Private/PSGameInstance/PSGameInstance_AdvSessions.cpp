#include "PSGameInstance/PSGameInstance_AdvSessions.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include <Online/OnlineSessionNames.h>

namespace PSSessionKeys
{
	static const FName SessionName(TEXT("GameSession"));

	static const FName KeyBuild(TEXT("BUILD"));
	static const FName KeySessionCode(TEXT("SESSION_CODE"));
	static const FName KeyHostName(TEXT("HOST_NAME"));
	static const FName KeyMode(TEXT("MODE"));
	static const FName KeyState(TEXT("STATE"));
	static const FName KeyMapPath(TEXT("MAP_PATH"));
}

namespace
{
	static IOnlineSubsystem* GetPSSessionSubsystem(const UObject* WorldContextObject, const FName& SubsystemName = NAME_None)
	{
		const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
		return Online::GetSubsystem(World, SubsystemName);
	}

	static FString GetOSSName(IOnlineSubsystem* OSS)
	{
		return OSS ? OSS->GetSubsystemName().ToString() : TEXT("NULL");
	}

	static FString GetOSSAppId(IOnlineSubsystem* OSS)
	{
		return OSS ? OSS->GetAppId() : TEXT("NONE");
	}

	static FString GetNetDriverName(const UObject* WorldContextObject)
	{
		const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
		const UNetDriver* NetDriver = World ? World->GetNetDriver() : nullptr;
		return NetDriver ? NetDriver->GetClass()->GetPathName() : TEXT("None");
	}

	static void LogPSSessionDiagnostics(const UObject* WorldContextObject, const TCHAR* Context)
	{
		IOnlineSubsystem* DefaultOSS = GetPSSessionSubsystem(WorldContextObject);
		IOnlineSubsystem* SteamOSS = GetPSSessionSubsystem(WorldContextObject, FName(TEXT("Steam")));
		const IOnlineSessionPtr SessionInterface = DefaultOSS ? DefaultOSS->GetSessionInterface() : nullptr;

		UE_LOG(LogTemp, Warning,
			TEXT("[PSSessions] OSS diagnostics (%s): Default=%s AppId=%s SessionInterface=%d Steam=%s SteamAppId=%s NetDriver=%s"),
			Context,
			*GetOSSName(DefaultOSS),
			*GetOSSAppId(DefaultOSS),
			SessionInterface.IsValid() ? 1 : 0,
			*GetOSSName(SteamOSS),
			*GetOSSAppId(SteamOSS),
			*GetNetDriverName(WorldContextObject));
	}
}

void UPSGameInstance_AdvSessions::Init()
{
	Super::Init();

	LogPSSessionDiagnostics(this, TEXT("Init"));
}

void UPSGameInstance_AdvSessions::Shutdown()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
		GEngine->OnTravelFailure().RemoveAll(this);
	}

	Super::Shutdown();
}

void UPSGameInstance_AdvSessions::SetPlayerSettings(int32 PlayerId, const FPlayerSetting& Settings)
{
	PlayerSettingsById.Add(PlayerId, Settings);
}

bool UPSGameInstance_AdvSessions::GetPlayerSettings(int32 PlayerId, FPlayerSetting& OutSettings) const
{
	if (const FPlayerSetting* Found = PlayerSettingsById.Find(PlayerId))
	{
		OutSettings = *Found;
		return true;
	}

	return false;
}

bool UPSGameInstance_AdvSessions::IsSessionFlowBusy() const
{
	switch (SessionFlowState)
	{
	case EPSessionFlowState::Finding:
	case EPSessionFlowState::Creating:
	case EPSessionFlowState::Joining:
	case EPSessionFlowState::Destroying:
	case EPSessionFlowState::Traveling:
		return true;
	default:
		return false;
	}
}

bool UPSGameInstance_AdvSessions::HasExistingNamedSession() const
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (!OSS)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return false;
	}

	return SessionInterface->GetNamedSession(PSSessionKeys::SessionName) != nullptr;
}

void UPSGameInstance_AdvSessions::SetFlowState(EPSessionFlowState NewState)
{
	if (SessionFlowState == NewState)
	{
		return;
	}

	const EPSessionFlowState OldState = SessionFlowState;
	SessionFlowState = NewState;
	OnSessionFlowStateChanged.Broadcast(OldState, NewState);
}

void UPSGameInstance_AdvSessions::ClearSessionError()
{
	LastSessionError = EPSessionFlowError::None;
	LastSessionErrorMessage.Empty();
}

void UPSGameInstance_AdvSessions::RaiseSessionError(EPSessionFlowError Error, const FString& Message)
{
	LastSessionError = Error;
	LastSessionErrorMessage = Message;
	OnSessionErrorRaised.Broadcast(Error, Message);
}

FString UPSGameInstance_AdvSessions::GenerateSessionCode() const
{
	static const FString Alphabet = TEXT("ABCDEFGHJKLMNPQRSTUVWXYZ23456789");
	FString Out;

	for (int32 i = 0; i < 5; ++i)
	{
		const int32 Index = FMath::RandRange(0, Alphabet.Len() - 1);
		Out.AppendChar(Alphabet[Index]);
	}

	return Out;
}

FString UPSGameInstance_AdvSessions::GetLocalHostName() const
{
	if (APlayerController* PC = GetFirstLocalPlayerController())
	{
		if (APlayerState* PS = PC->PlayerState)
		{
			const FString Name = PS->GetPlayerName();
			if (!Name.IsEmpty())
			{
				return Name;
			}
		}
	}

	if (const ULocalPlayer* LP = GetFirstGamePlayer())
	{
		const FString Nick = LP->GetNickname();
		if (!Nick.IsEmpty())
		{
			return Nick;
		}
	}

	return TEXT("Host");
}

bool UPSGameInstance_AdvSessions::IsSearchResultFresh(const FString& StableId) const
{
	const double* FoundAt = SearchResultFoundAtById.Find(StableId);
	if (!FoundAt)
	{
		return false;
	}

	const double Now = FPlatformTime::Seconds();
	return (Now - *FoundAt) <= static_cast<double>(LastSearchRequest.MaxEntryAgeSeconds);
}

bool UPSGameInstance_AdvSessions::RequestRefreshSessions(const FSessionSearchRequest& Request)
{
	if (IsSessionFlowBusy())
	{
		RaiseSessionError(EPSessionFlowError::Busy, TEXT("Session system is busy."));
		return false;
	}

	return StartFindSessions(Request);
}

bool UPSGameInstance_AdvSessions::RequestHostSession(const FHostSessionRequest& Request)
{
	if (IsSessionFlowBusy())
	{
		RaiseSessionError(EPSessionFlowError::Busy, TEXT("Session system is busy."));
		return false;
	}

	if (Request.LobbyMapName.IsEmpty())
	{
		RaiseSessionError(EPSessionFlowError::InvalidRequest, TEXT("LobbyMapName is empty."));
		return false;
	}

	if (HasExistingNamedSession())
	{
		PendingHostRequest = Request;
		DeferredAction = EDeferredSessionAction::Create;
		return BeginDestroyCurrentSession(true);
	}

	return StartCreateSession(Request);
}

bool UPSGameInstance_AdvSessions::RequestJoinSessionById(const FString& StableId)
{
	if (IsSessionFlowBusy())
	{
		RaiseSessionError(EPSessionFlowError::Busy, TEXT("Session system is busy."));
		return false;
	}

	if (StableId.IsEmpty())
	{
		RaiseSessionError(EPSessionFlowError::InvalidRequest, TEXT("StableId is empty."));
		return false;
	}

	if (HasExistingNamedSession())
	{
		PendingJoinStableId = StableId;
		DeferredAction = EDeferredSessionAction::Join;
		return BeginDestroyCurrentSession(true);
	}

	return StartJoinSessionById(StableId);
}

bool UPSGameInstance_AdvSessions::RequestJoinSessionByCode(const FString& SessionCode)
{
	for (const FSessionBrowserEntry& Entry : CachedBrowserEntries)
	{
		if (Entry.SessionCode.Equals(SessionCode, ESearchCase::IgnoreCase))
		{
			return RequestJoinSessionById(Entry.StableId);
		}
	}

	RaiseSessionError(EPSessionFlowError::ResultNotFound, TEXT("No cached session matched that session code."));
	return false;
}

bool UPSGameInstance_AdvSessions::RequestDestroyCurrentSession()
{
	if (IsSessionFlowBusy())
	{
		RaiseSessionError(EPSessionFlowError::Busy, TEXT("Session system is busy."));
		return false;
	}

	DeferredAction = EDeferredSessionAction::None;
	PendingJoinStableId.Empty();
	return BeginDestroyCurrentSession(false);
}

void UPSGameInstance_AdvSessions::NotifyEnteredLobby()
{
	SetFlowState(EPSessionFlowState::InLobby);
}

void UPSGameInstance_AdvSessions::NotifyEnteredMatch()
{
	SetFlowState(EPSessionFlowState::InMatch);
}

void UPSGameInstance_AdvSessions::NotifyReturnedToMenu()
{
	SetFlowState(EPSessionFlowState::Idle);
}

bool UPSGameInstance_AdvSessions::StartFindSessions(const FSessionSearchRequest& Request)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (!OSS)
	{
		LogPSSessionDiagnostics(this, TEXT("StartFindSessions.NoSubsystem"));
		RaiseSessionError(EPSessionFlowError::NoSubsystem, TEXT("No online subsystem found."));
		return false;
	}

	IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		LogPSSessionDiagnostics(this, TEXT("StartFindSessions.NoSessionInterface"));
		RaiseSessionError(EPSessionFlowError::NoSessionInterface, TEXT("No online session interface found."));
		return false;
	}

	ClearSessionError();
	LastSearchRequest = Request;

	CachedBrowserEntries.Empty();
	CachedSearchResultsById.Empty();
	SearchResultFoundAtById.Empty();

	CurrentSessionSearch = MakeShared<FOnlineSessionSearch>();
	CurrentSessionSearch->bIsLanQuery = Request.bIsLAN;
	CurrentSessionSearch->MaxSearchResults = Request.MaxResults;

	if (Request.bUsePresence)
	{
		CurrentSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		CurrentSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	FindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPSGameInstance_AdvSessions::HandleFindSessionsComplete)
	);

	SetFlowState(EPSessionFlowState::Finding);

	if (!SessionInterface->FindSessions(0, CurrentSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		FindSessionsCompleteHandle.Reset();

		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::FindFailed, TEXT("FindSessions failed to start."));
		return false;
	}

	return true;
}

bool UPSGameInstance_AdvSessions::StartCreateSession(const FHostSessionRequest& Request)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (!OSS)
	{
		LogPSSessionDiagnostics(this, TEXT("StartCreateSession.NoSubsystem"));
		RaiseSessionError(EPSessionFlowError::NoSubsystem, TEXT("No online subsystem found."));
		return false;
	}

	IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		LogPSSessionDiagnostics(this, TEXT("StartCreateSession.NoSessionInterface"));
		RaiseSessionError(EPSessionFlowError::NoSessionInterface, TEXT("No online session interface found."));
		return false;
	}

	ClearSessionError();
	PendingHostRequest = Request;

	PendingHostRequest.bUsePresence = true;
	PendingHostRequest.bUseLobbiesIfAvailable = true;

	if (PendingHostRequest.SessionCode.IsEmpty())
	{
		PendingHostRequest.SessionCode = GenerateSessionCode();
	}

	FOnlineSessionSettings Settings;
	Settings.bIsLANMatch = PendingHostRequest.bIsLAN;
	Settings.NumPublicConnections = PendingHostRequest.PublicConnections;
	Settings.bShouldAdvertise = PendingHostRequest.bShouldAdvertise;
	Settings.bAllowJoinInProgress = PendingHostRequest.bAllowJoinInProgress;
	Settings.bAllowInvites = PendingHostRequest.bAllowInvites;
	Settings.bUsesPresence = PendingHostRequest.bUsePresence;
	Settings.bUseLobbiesIfAvailable = PendingHostRequest.bUseLobbiesIfAvailable;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinViaPresenceFriendsOnly = false;
	Settings.BuildUniqueId = PendingHostRequest.BuildId;

	Settings.Set(PSSessionKeys::KeyBuild, PendingHostRequest.BuildId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(PSSessionKeys::KeySessionCode, PendingHostRequest.SessionCode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(PSSessionKeys::KeyHostName, GetLocalHostName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(PSSessionKeys::KeyMode, PendingHostRequest.ModeName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(PSSessionKeys::KeyState, FString(TEXT("Lobby")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(PSSessionKeys::KeyMapPath, PendingHostRequest.LobbyMapName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(SETTING_MAPNAME, PendingHostRequest.DisplayMapName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPSGameInstance_AdvSessions::HandleCreateSessionComplete)
	);

	SetFlowState(EPSessionFlowState::Creating);

	if (!SessionInterface->CreateSession(0, PSSessionKeys::SessionName, Settings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		CreateSessionCompleteHandle.Reset();

		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::CreateFailed, TEXT("CreateSession failed to start."));
		return false;
	}

	return true;
}

bool UPSGameInstance_AdvSessions::StartJoinSessionById(const FString& StableId)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (!OSS)
	{
		LogPSSessionDiagnostics(this, TEXT("StartJoinSessionById.NoSubsystem"));
		RaiseSessionError(EPSessionFlowError::NoSubsystem, TEXT("No online subsystem found."));
		return false;
	}

	IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		LogPSSessionDiagnostics(this, TEXT("StartJoinSessionById.NoSessionInterface"));
		RaiseSessionError(EPSessionFlowError::NoSessionInterface, TEXT("No online session interface found."));
		return false;
	}

	const FOnlineSessionSearchResult* SearchResult = CachedSearchResultsById.Find(StableId);
	if (!SearchResult)
	{
		RaiseSessionError(EPSessionFlowError::ResultNotFound, TEXT("Selected browser entry is missing from cache."));
		return false;
	}

	if (!IsSearchResultFresh(StableId))
	{
		RaiseSessionError(EPSessionFlowError::ResultStale, TEXT("Selected browser entry is stale. Refresh sessions and try again."));
		return false;
	}

	for (const FSessionBrowserEntry& Entry : CachedBrowserEntries)
	{
		if (Entry.StableId == StableId)
		{
			if (!Entry.bIsSameBuild)
			{
				RaiseSessionError(EPSessionFlowError::BuildMismatch, TEXT("Selected session is from a different build."));
				return false;
			}

			if (!Entry.bIsJoinable)
			{
				RaiseSessionError(EPSessionFlowError::JoinRejected, TEXT("Selected session is no longer joinable."));
				return false;
			}

			break;
		}
	}

	ClearSessionError();
	PendingJoinStableId = StableId;

	JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPSGameInstance_AdvSessions::HandleJoinSessionComplete)
	);

	SetFlowState(EPSessionFlowState::Joining);

	FOnlineSessionSearchResult SearchResultToJoin = *SearchResult;

	SearchResultToJoin.Session.SessionSettings.bUsesPresence = true;
	SearchResultToJoin.Session.SessionSettings.bUseLobbiesIfAvailable = true;
	SearchResultToJoin.Session.SessionSettings.bAllowJoinViaPresence = true;
	SearchResultToJoin.Session.SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

	UE_LOG(LogTemp, Warning,
		TEXT("[PSSessions] Joining StableId=%s Presence=%d Lobbies=%d OpenPublic=%d"),
		*StableId,
		SearchResultToJoin.Session.SessionSettings.bUsesPresence ? 1 : 0,
		SearchResultToJoin.Session.SessionSettings.bUseLobbiesIfAvailable ? 1 : 0,
		SearchResultToJoin.Session.NumOpenPublicConnections
	);

	if (!SessionInterface->JoinSession(0, PSSessionKeys::SessionName, SearchResultToJoin))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		JoinSessionCompleteHandle.Reset();

		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::JoinFailed, TEXT("JoinSession failed to start."));
		return false;
	}

	return true;
}

bool UPSGameInstance_AdvSessions::BeginDestroyCurrentSession(bool bFromDeferredFlow)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (!OSS)
	{
		LogPSSessionDiagnostics(this, TEXT("BeginDestroyCurrentSession.NoSubsystem"));
		RaiseSessionError(EPSessionFlowError::NoSubsystem, TEXT("No online subsystem found."));
		return false;
	}

	IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		LogPSSessionDiagnostics(this, TEXT("BeginDestroyCurrentSession.NoSessionInterface"));
		RaiseSessionError(EPSessionFlowError::NoSessionInterface, TEXT("No online session interface found."));
		return false;
	}

	if (!HasExistingNamedSession())
	{
		if (bFromDeferredFlow)
		{
			ExecuteDeferredActionIfAny();
			return true;
		}

		RaiseSessionError(EPSessionFlowError::DestroyFailed, TEXT("No existing named session to destroy."));
		return false;
	}

	ClearSessionError();

	DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPSGameInstance_AdvSessions::HandleDestroySessionComplete)
	);

	SetFlowState(EPSessionFlowState::Destroying);

	if (!SessionInterface->DestroySession(PSSessionKeys::SessionName))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
		DestroySessionCompleteHandle.Reset();

		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::DestroyFailed, TEXT("DestroySession failed to start."));
		return false;
	}

	return true;
}

void UPSGameInstance_AdvSessions::ExecuteDeferredActionIfAny()
{
	const EDeferredSessionAction ActionToRun = DeferredAction;
	DeferredAction = EDeferredSessionAction::None;

	switch (ActionToRun)
	{
	case EDeferredSessionAction::Create:
		StartCreateSession(PendingHostRequest);
		break;

	case EDeferredSessionAction::Join:
		StartJoinSessionById(PendingJoinStableId);
		break;

	default:
		break;
	}
}

void UPSGameInstance_AdvSessions::BuildBrowserEntriesFromSearch()
{
	CachedBrowserEntries.Empty();
	CachedSearchResultsById.Empty();
	SearchResultFoundAtById.Empty();

	if (!CurrentSessionSearch.IsValid())
	{
		return;
	}

	const double Now = FPlatformTime::Seconds();
	TSet<FString> SeenIds;

	for (const FOnlineSessionSearchResult& Result : CurrentSessionSearch->SearchResults)
	{
		FSessionBrowserEntry Entry;

		Entry.StableId = Result.GetSessionIdStr();
		if (Entry.StableId.IsEmpty())
		{
			continue;
		}

		if (SeenIds.Contains(Entry.StableId))
		{
			continue;
		}
		SeenIds.Add(Entry.StableId);

		Entry.HostName = Result.Session.OwningUserName;
		Entry.PingMs = Result.PingInMs;
		Entry.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		Entry.CurrentPlayers = Entry.MaxPlayers - Result.Session.NumOpenPublicConnections;
		Entry.BuildId = Result.Session.SessionSettings.BuildUniqueId;
		Entry.FoundAtSeconds = static_cast<float>(Now);

		Result.Session.SessionSettings.Get(PSSessionKeys::KeySessionCode, Entry.SessionCode);
		Result.Session.SessionSettings.Get(PSSessionKeys::KeyHostName, Entry.HostName);
		Result.Session.SessionSettings.Get(SETTING_MAPNAME, Entry.MapName);
		Result.Session.SessionSettings.Get(PSSessionKeys::KeyMode, Entry.ModeName);
		Result.Session.SessionSettings.Get(PSSessionKeys::KeyState, Entry.SessionState);
		Result.Session.SessionSettings.Get(PSSessionKeys::KeyBuild, Entry.BuildId);

		Entry.bIsSameBuild = (Entry.BuildId == LastSearchRequest.RequiredBuildId);
		Entry.bIsJoinable = (Result.Session.NumOpenPublicConnections > 0);

		if (Entry.SessionState.IsEmpty())
		{
			Entry.SessionState = TEXT("Unknown");
		}

		if (LastSearchRequest.bOnlySameBuild && !Entry.bIsSameBuild)
		{
			continue;
		}

		if (LastSearchRequest.bOnlyJoinable && !Entry.bIsJoinable)
		{
			continue;
		}

		if (LastSearchRequest.bOnlyLobbyState && !Entry.SessionState.Equals(TEXT("Lobby"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		CachedBrowserEntries.Add(Entry);

		FOnlineSessionSearchResult JoinableResult = Result;
		JoinableResult.Session.SessionSettings.bUsesPresence = true;
		JoinableResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;
		JoinableResult.Session.SessionSettings.bAllowJoinViaPresence = true;
		JoinableResult.Session.SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

		CachedSearchResultsById.Add(Entry.StableId, JoinableResult);
		SearchResultFoundAtById.Add(Entry.StableId, Now);
	}

	CachedBrowserEntries.Sort([](const FSessionBrowserEntry& A, const FSessionBrowserEntry& B)
		{
			if (A.bIsSameBuild != B.bIsSameBuild)
			{
				return A.bIsSameBuild && !B.bIsSameBuild;
			}

			if (A.bIsJoinable != B.bIsJoinable)
			{
				return A.bIsJoinable && !B.bIsJoinable;
			}

			return A.PingMs < B.PingMs;
		});
}

void UPSGameInstance_AdvSessions::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);

	UE_LOG(LogTemp, Warning, TEXT("[PSSessions] Create complete. Success=%d OSS=%s SessionName=%s"),
		bWasSuccessful ? 1 : 0,
		OSS ? *OSS->GetSubsystemName().ToString() : TEXT("NULL_OSS"),
		*SessionName.ToString());

	if (OSS)
	{
		if (IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface())
		{
			if (FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(PSSessionKeys::SessionName))
			{
				UE_LOG(LogTemp, Error,
					TEXT("[PSSessions] Named session exists. OwningUser=%s OpenPublic=%d NumPublic=%d LAN=%d Presence=%d Lobby=%d Advertise=%d"),
					*NamedSession->OwningUserName,
					NamedSession->NumOpenPublicConnections,
					NamedSession->SessionSettings.NumPublicConnections,
					NamedSession->SessionSettings.bIsLANMatch ? 1 : 0,
					NamedSession->SessionSettings.bUsesPresence ? 1 : 0,
					NamedSession->SessionSettings.bUseLobbiesIfAvailable ? 1 : 0,
					NamedSession->SessionSettings.bShouldAdvertise ? 1 : 0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[PSSessions] Create succeeded but GetNamedSession returned null."));
			}

			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		}
	}

	CreateSessionCompleteHandle.Reset();

	if (!bWasSuccessful)
	{
		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::CreateFailed, TEXT("CreateSession completed with failure."));
		return;
	}

	SetFlowState(EPSessionFlowState::Traveling);

	UGameplayStatics::OpenLevel(
		this,
		FName(*PendingHostRequest.LobbyMapName),
		true,
		TEXT("listen")
	);
}

void UPSGameInstance_AdvSessions::HandleFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	IOnlineSessionPtr SessionInterface;

	if (OSS)
	{
		SessionInterface = OSS->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		}
	}

	FindSessionsCompleteHandle.Reset();

	const int32 RawResultCount = CurrentSessionSearch.IsValid()
		? CurrentSessionSearch->SearchResults.Num()
		: -1;

	UE_LOG(LogTemp, Warning, TEXT("[PSSessions] Find complete. Success=%d RawResults=%d"),
		bWasSuccessful ? 1 : 0,
		RawResultCount);

	SetFlowState(EPSessionFlowState::Idle);

	if (!bWasSuccessful)
	{
		RaiseSessionError(EPSessionFlowError::FindFailed, TEXT("FindSessions completed with failure."));
		OnSessionBrowserUpdated.Broadcast();
		return;
	}

	ClearSessionError();
	BuildBrowserEntriesFromSearch();

	UE_LOG(LogTemp, Warning, TEXT("[PSSessions] Cached entries after filtering: %d"), CachedBrowserEntries.Num());

	OnSessionBrowserUpdated.Broadcast();
}

void UPSGameInstance_AdvSessions::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	IOnlineSessionPtr SessionInterface;
	if (OSS)
	{
		SessionInterface = OSS->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		}
	}
	JoinSessionCompleteHandle.Reset();

	if (!SessionInterface.IsValid())
	{
		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::NoSessionInterface, TEXT("Session interface missing after join."));
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		SetFlowState(EPSessionFlowState::Idle);

		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			RaiseSessionError(EPSessionFlowError::JoinRejected, TEXT("Session is full."));
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			RaiseSessionError(EPSessionFlowError::JoinRejected, TEXT("Session no longer exists."));
			break;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			RaiseSessionError(EPSessionFlowError::NoConnectString, TEXT("Could not retrieve server address."));
			break;
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			RaiseSessionError(EPSessionFlowError::JoinRejected, TEXT("Already in a session."));
			break;
		default:
			RaiseSessionError(EPSessionFlowError::JoinFailed, TEXT("JoinSession completed with failure."));
			break;
		}

		return;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(PSSessionKeys::SessionName, ConnectString) || ConnectString.IsEmpty())
	{
		SetFlowState(EPSessionFlowState::Idle);
		RaiseSessionError(EPSessionFlowError::NoConnectString, TEXT("Resolved connect string was empty."));
		return;
	}

	if (APlayerController* PC = GetFirstLocalPlayerController())
	{
		SetFlowState(EPSessionFlowState::Traveling);
		PC->ClientTravel(ConnectString, TRAVEL_Absolute);
		return;
	}

	SetFlowState(EPSessionFlowState::Idle);
	RaiseSessionError(EPSessionFlowError::TravelFailure, TEXT("No local player controller available for ClientTravel."));
}

void UPSGameInstance_AdvSessions::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OSS = GetPSSessionSubsystem(this);
	if (OSS)
	{
		if (IOnlineSessionPtr SessionInterface = OSS->GetSessionInterface())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
		}
	}
	DestroySessionCompleteHandle.Reset();

	SetFlowState(EPSessionFlowState::Idle);

	if (!bWasSuccessful)
	{
		RaiseSessionError(EPSessionFlowError::DestroyFailed, TEXT("DestroySession completed with failure."));
		DeferredAction = EDeferredSessionAction::None;
		return;
	}

	ClearSessionError();
	ExecuteDeferredActionIfAny();
}

void UPSGameInstance_AdvSessions::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	SetFlowState(EPSessionFlowState::Idle);
	RaiseSessionError(EPSessionFlowError::NetworkFailure, ErrorString);
}

void UPSGameInstance_AdvSessions::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	SetFlowState(EPSessionFlowState::Idle);
	RaiseSessionError(EPSessionFlowError::TravelFailure, ErrorString);
}
