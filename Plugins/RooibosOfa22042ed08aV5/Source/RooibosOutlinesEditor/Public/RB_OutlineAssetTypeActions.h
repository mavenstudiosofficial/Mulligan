// Copyright Joram Wessels 2023

#pragma once
#include "AssetTypeActions_Base.h"

class FRB_OutlineAssetTypeActions : public FAssetTypeActions_Base
{
	
public:
	
	FRB_OutlineAssetTypeActions(EAssetTypeCategories::Type InAssetCategory, const TArray<FText>& InSubCategories)
	  : category(InAssetCategory), subCategories(InSubCategories) {}
    
	// IAssetTypeActions interface
	virtual FText GetName() const override { return FText::FromString(TEXT("Outline Style")); }
	virtual FColor GetTypeColor() const override { return FColor(100, 100, 100); }
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override { return category; }
	virtual const TArray<FText>& GetSubMenus() const override { return subCategories; }
	// End of IAssetTypeActions interface

private:
	
	EAssetTypeCategories::Type category;

	TArray<FText> subCategories;
	
};
