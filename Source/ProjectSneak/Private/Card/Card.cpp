// Fill out your copyright notice in the Description page of Project Settings.


#include "Card/Card.h"
#include "Card/CardHolder.h"

void UCard::PostInitProperties()
{
	Super::PostInitProperties();
	cardId = FGuid::NewGuid();
}

bool UCard::PlaySelf()
{
	if (!holder)
	{
		return false;
	}

	auto holderPtr = holder.Get();
	return holderPtr->PlayCardByPtr(this);
}

bool UCard::DrawSelf()
{
	if (!holder)
	{
		return false;
	}

	auto holderPtr = holder.Get();
	return holderPtr->DrawSelectedCard(this);
}

UTexture2D* UCard::GetTexture()
{
	return texture;
}