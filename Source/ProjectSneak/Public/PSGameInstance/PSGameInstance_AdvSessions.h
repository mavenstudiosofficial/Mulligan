#pragma once

#include "CoreMinimal.h"
#include "PlayerSettingsTypes.h"
#include "AdvancedFriendsGameInstance.h"
#include "PSSessionsTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Engine/EngineBaseTypes.h"
#include "PSGameInstance_AdvSessions.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionBrowserUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSessionFlowStateChanged,
	EPSessionFlowState, OldState,
	EPSessionFlowState, NewState
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSessionErrorRaised,
	EPSessionFlowError, Error,
	const FString&, Message
);

class UNetDriver;

UCLASS()
class PROJECTSNEAK_API UPSGameInstance_AdvSessions : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Player Settings")
	TMap<int32, FPlayerSetting> PlayerSettingsById;

	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetPlayerSettings(int32 PlayerId, const FPlayerSetting& Settings);

	UFUNCTION(BlueprintPure, Category = "Player Settings")
	bool GetPlayerSettings(int32 PlayerId, FPlayerSetting& OutSettings) const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	EPSessionFlowState SessionFlowState = EPSessionFlowState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	EPSessionFlowError LastSessionError = EPSessionFlowError::None;

	UPROPERTY(BlueprintReadOnly, Category = "Sessions")
	FString LastSessionErrorMessage;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FOnSessionBrowserUpdated OnSessionBrowserUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FOnSessionFlowStateChanged OnSessionFlowStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FOnSessionErrorRaised OnSessionErrorRaised;

public:
	UFUNCTION(BlueprintCallable, Category = "Sessions")
	bool RequestRefreshSessions(const FSessionSearchRequest& Request);

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	bool RequestHostSession(const FHostSessionRequest& Request);

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	bool RequestJoinSessionById(const FString& StableId);

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	bool RequestJoinSessionByCode(const FString& SessionCode);

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	bool RequestDestroyCurrentSession();

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void NotifyEnteredLobby();

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void NotifyEnteredMatch();

	UFUNCTION(BlueprintCallable, Category = "Sessions")
	void NotifyReturnedToMenu();

	UFUNCTION(BlueprintPure, Category = "Sessions")
	bool IsSessionFlowBusy() const;

	UFUNCTION(BlueprintPure, Category = "Sessions")
	TArray<FSessionBrowserEntry> GetCachedBrowserEntries() const { return CachedBrowserEntries; }

	UFUNCTION(BlueprintPure, Category = "Sessions")
	EPSessionFlowError GetLastSessionError() const { return LastSessionError; }

	UFUNCTION(BlueprintPure, Category = "Sessions")
	FString GetLastSessionErrorMessage() const { return LastSessionErrorMessage; }

private:
	enum class EDeferredSessionAction : uint8
	{
		None,
		Create,
		Join
	};

private:
	bool StartFindSessions(const FSessionSearchRequest& Request);
	bool StartCreateSession(const FHostSessionRequest& Request);
	bool StartJoinSessionById(const FString& StableId);
	bool BeginDestroyCurrentSession(bool bFromDeferredFlow);
	void ExecuteDeferredActionIfAny();

	void SetFlowState(EPSessionFlowState NewState);
	void RaiseSessionError(EPSessionFlowError Error, const FString& Message);
	void ClearSessionError();

	bool HasExistingNamedSession() const;
	bool IsSearchResultFresh(const FString& StableId) const;
	FString GenerateSessionCode() const;
	FString GetLocalHostName() const;

	void BuildBrowserEntriesFromSearch();

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleFindSessionsComplete(bool bWasSuccessful);
	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

private:
	FHostSessionRequest PendingHostRequest;
	FSessionSearchRequest LastSearchRequest;
	FString PendingJoinStableId;

	EDeferredSessionAction DeferredAction = EDeferredSessionAction::None;

	TArray<FSessionBrowserEntry> CachedBrowserEntries;

	TMap<FString, FOnlineSessionSearchResult> CachedSearchResultsById;
	TMap<FString, double> SearchResultFoundAtById;

	TSharedPtr<FOnlineSessionSearch> CurrentSessionSearch;

	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle FindSessionsCompleteHandle;
	FDelegateHandle JoinSessionCompleteHandle;
	FDelegateHandle DestroySessionCompleteHandle;
};