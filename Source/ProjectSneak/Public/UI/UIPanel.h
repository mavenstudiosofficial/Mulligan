// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "UIPanel.generated.h"

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EUIPanelBehavior : uint32 {
	None = 0 UMETA(Hidden),
	ShowUIMask = 0x01,
	HostInput = 0x02,
	DelayedShow = 0x04,
	HideOthersPanel = 0x08,
	ToastPanel = 0x16,
	Singleton = 0x32
};

ENUM_CLASS_FLAGS(EUIPanelBehavior);
/**
 *
 */
UCLASS()
class PROJECTSNEAK_API UUIPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventCalled);

	FGuid PanelId;

	UPROPERTY(EditAnywhere, Category = "UIPanel")
	int PanelOrder;

	UPROPERTY(EditAnywhere, Category = "UIPanel")
	EUIPanelBehavior PanelBehavior;

	FTimerHandle ToastTimerHandle;

	UUIPanel(const FObjectInitializer& ObjectInitializer);

	void InitPanel();

	void Show();

	void Hide();

	void Close();

	int CompareTo(UUIPanel InOther);

	UFUNCTION(BlueprintCallable, Category = "UIPanel")
	void InsertChildAtStart(UOverlay* Parent, UWidget* NewChild);

protected:
	UPROPERTY(BlueprintAssignable, Category = "UIPanel")
	FOnEventCalled OnShow;

	UPROPERTY(BlueprintAssignable, Category = "UIPanel")
	FOnEventCalled OnHide;

	UPROPERTY(BlueprintAssignable, Category = "UIPanel")
	FOnEventCalled OnClose;

private:
	const float DEFAULT_TOAST_TIME = 5;
};
