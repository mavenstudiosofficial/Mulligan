// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIPanel.h"
#include "SubSystems/Keys.h"

UUIPanel::UUIPanel(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	PanelId = FGuid::NewGuid();
	PanelOrder = 0;
	PanelBehavior = EUIPanelBehavior::None;
}

void UUIPanel::InitPanel()
{
	if ((PanelBehavior & EUIPanelBehavior::ToastPanel) == EUIPanelBehavior::ToastPanel)
	{
		GetWorld()->GetTimerManager().SetTimer(ToastTimerHandle, DEFAULT_TOAST_TIME, false);
	}
}

void UUIPanel::Show()
{
	SetVisibility(ESlateVisibility::Visible);
	OnShow.Broadcast();
}

void UUIPanel::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnHide.Broadcast();
}

void UUIPanel::Close()
{
	OnClose.Broadcast();
	this->RemoveFromParent();
}

int UUIPanel::CompareTo(UUIPanel InOther)
{
	return PanelOrder - InOther.PanelOrder;
}

void UUIPanel::InsertChildAtStart(UOverlay* Parent, UWidget* NewChild)
{
	TArray<UWidget*> CurrentChildren = Parent->GetAllChildren();
	Parent->ClearChildren();
	Parent->AddChild(NewChild);

	for (int32 i = 0; i < CurrentChildren.Num(); i++)
	{
		Parent->AddChild(CurrentChildren[i]);
	}
}
