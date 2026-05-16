#include "PSPlayerState/PSPlayerState.h"
#include "ProjectSneakCharacter.h"
#include "Data/PSSaveGame.h"
#include "Card/CardHolder.h"
#include "Card/Card.h"
#include "Kismet/GameplayStatics.h"
#include <Net/UnrealNetwork.h>


void APSPlayerState::UploadReadyState_Implementation(bool isReady)
{
	bIsReady = isReady;
	OnReadyStateChanged.Broadcast(bIsReady);
}

void APSPlayerState::UploadPlayerSetting_Implementation(const FPlayerSetting& setting)
{
	PlayerSetting = setting;
}

void APSPlayerState::RefreshPlayerSetting(APawn* playerPawn)
{
	if (!playerPawn)
	{
		playerPawn = GetPawn();
	}
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (!cardHolder)
	{
		return;
	}

	PlayerSetting.deck.Empty();

	for (auto& card : cardHolder->deckCards)
	{
		PlayerSetting.deck.Add(card->cardId);
	}
}

void APSPlayerState::RefreshPlayerSave(APawn* playerPawn)
{
	if (!playerPawn)
	{
		playerPawn = GetPawn();
	}
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	PlayerSave.coins = player->GetCoinCount();
	PlayerSave.gems = player->GetGemCount();

	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (cardHolder)
	{
		PlayerSave.cardInventory = cardHolder->cardInventory;
	}

	// Save Deck Preset if card holder is locked (not in dungeon)
	if (!cardHolder->isLocked)
	{
		return;
	}

	auto character = player->characterClass;
	PlayerSave.selectedCharacter = character;
	FCharacterSave characterSave = PlayerSave.characterSet.FindOrAdd(character);
	characterSave.deckPreset.Empty();

	for (auto& card : cardHolder->deckCards)
	{
		characterSave.deckPreset.Add(card->cardId);
	}

	PlayerSave.characterSet[character] = characterSave;
}

bool APSPlayerState::HasSaveData(int playerIndex)
{
	return UGameplayStatics::DoesSaveGameExist(TEXT("save00"), playerIndex);;
}

void APSPlayerState::SavePlayerData(int playerIndex)
{
	RefreshPlayerSave();

	if (UPSSaveGame* save = Cast<UPSSaveGame>(UGameplayStatics::CreateSaveGameObject(UPSSaveGame::StaticClass())))
	{
		save->playerSave = PlayerSave;
		UGameplayStatics::AsyncSaveGameToSlot(save, TEXT("save00"), playerIndex);
	}
}

void APSPlayerState::LoadPlayerData(int playerIndex)
{
	UPSSaveGame* save = Cast<UPSSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("save00"), playerIndex));
	if (!save)
	{
		return;
	}

	FPlayerSave playerSave = save->playerSave;
	ReloadFromSave(playerSave);

	// Load character & card preset
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	FCharacterSave characterSave = playerSave.characterSet.FindOrAdd(playerSave.selectedCharacter);
	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (cardHolder)
	{
		for (auto& cardId : characterSave.deckPreset)
		{
			int outIndex;
			cardHolder->AddCardToDeck(cardId, outIndex);
		}
	}
}

void APSPlayerState::ReloadFromCache(const FPlayerSave& saveData, const FPlayerSetting& settingData)
{
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		FTimerManager& timerManager = GetWorldTimerManager();
		timerManager.SetTimerForNextTick([this, saveData, settingData]()
			{
				if (!IsValid(this))
				{
					return;
				}
				ReloadFromCache(saveData, settingData);
			});
		return;
	}

	FTimerManager& timerManager = GetWorldTimerManager();
	timerManager.SetTimerForNextTick([this, saveData, settingData]()
		{
			if (!IsValid(this))
			{
				return;
			}

			ReloadFromSave(saveData);
			ReloadFromSetting(settingData);
		});
}

void APSPlayerState::ReloadFromSave(const FPlayerSave& saveData)
{
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	PlayerSave = saveData;

	player->ChangeCoinCount(saveData.coins);
	player->ChangeGemCount(saveData.gems);
	player->RequestSwitchCharacter(saveData.selectedCharacter);

	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (cardHolder)
	{
		cardHolder->cardInventory = saveData.cardInventory;
	}
}

void APSPlayerState::ReloadFromSetting(const FPlayerSetting& settingData)
{
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (!cardHolder)
	{
		return;
	}

	PlayerSetting = settingData;

	for (auto& cardId : settingData.deck)
	{
		int outIndex;
		if (!cardHolder->AddCardToDeck(cardId, outIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player state reload, failed to add card"));
		}
	}
}

void APSPlayerState::OnSwitchCharacter(EPlayerClass playerClass)
{
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	auto cardHolder = player->GetComponentByClass<UCardHolder>();
	if (!cardHolder)
	{
		return;
	}

	PlayerSave.selectedCharacter = playerClass;

	FCharacterSave save = PlayerSave.characterSet.FindOrAdd(playerClass);
	for (auto& cardId: save.deckPreset)
	{
		int outIndex;
		cardHolder->AddCardToDeck(cardId, outIndex);
	}
}


void APSPlayerState::CopyProperties(APlayerState* NewPS)
{
	Super::CopyProperties(NewPS);

	//auto* newState = Cast<APSPlayerState>(NewPS);
	//if (newState)
	//{
	//	newState->bIsReady = bIsReady;
	//	newState->PlayerSave = PlayerSave;
	//	newState->PlayerSetting = PlayerSetting;
	//}
}

void APSPlayerState::OverrideWith(APlayerState* OldPS)
{
	Super::OverrideWith(OldPS);

	//auto* oldState = Cast<APSPlayerState>(OldPS);
	//if (oldState)
	//{
	//	bIsReady = oldState->bIsReady;
	//	PlayerSave = oldState->PlayerSave;
	//	PlayerSetting = oldState->PlayerSetting;
	//}
}

void APSPlayerState::SavePlayerOptionSettings(int playerIndex, FPlayerOptionSettings newOptions)
{
	RefreshPlayerSave();

	if (UPSOptionsSave* save = Cast<UPSOptionsSave>(UGameplayStatics::CreateSaveGameObject(UPSOptionsSave::StaticClass())))
	{
		save->playerOptionsSettings = newOptions;
		UGameplayStatics::AsyncSaveGameToSlot(save, TEXT("save01"), playerIndex);
		PlayerOptionSettings = newOptions;
	}
}

void APSPlayerState::LoadPlayerOptionSettings(int playerIndex)
{
	UPSOptionsSave* save = Cast<UPSOptionsSave>(UGameplayStatics::LoadGameFromSlot(TEXT("save01"), playerIndex));
	if (!save)
	{
		return;
	}

	FPlayerOptionSettings playerOptionSettings = save->playerOptionsSettings;
	ReloadFromOptionSettings(playerOptionSettings);

}

void APSPlayerState::ReloadFromOptionSettings(FPlayerOptionSettings playerOptions)
{
	auto playerPawn = GetPawn();
	auto player = Cast<AProjectSneakCharacter>(playerPawn);

	if (!player)
	{
		return;
	}

	PlayerOptionSettings = playerOptions;
}