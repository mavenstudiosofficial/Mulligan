// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Wallet.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSNEAK_API UWallet : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWallet();

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	void GainGem(int amount);
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	void GainGold(int amount);

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	int GetGemAmount();
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	int GetGoldAmount();

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool HasEnoughGem(int amount);
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool HasEnoughGold(int amount);

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool TryUseGem(int amount);
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool TryUseGold(int amount);

protected:
	virtual void BeginPlay() override;

private:
	int m_gem;
	int m_gold;
};
