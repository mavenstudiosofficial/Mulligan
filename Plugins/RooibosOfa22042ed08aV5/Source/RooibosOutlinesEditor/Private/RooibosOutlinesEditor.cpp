// Copyright Joram Wessels 2023

#include "RooibosOutlinesEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "RB_OutlineAssetTypeActions.h"
#include "RB_OutlineSettings.h"
#include "RB_OutlineStyle.h"
#include "RB_OutlineThumbnailRenderer.h"
#include "Engine/RendererSettings.h"

#define LOCTEXT_NAMESPACE "FRooibosOutlinesEditorModule"

void FRooibosOutlinesEditorModule::StartupModule()
{
	// Get the asset tools module
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	// To register a custom category
	EAssetTypeCategories::Type rooibosCategory = assetTools.RegisterAdvancedAssetCategory(
		FName(TEXT("Rooibos")), FText::FromString("Rooibos"));
    
	// Name the subcategories it should appear in (if any)
	static const TArray<FText> subCategories = { FText::FromString(TEXT("Outlines")) };

	// Create instance and register with the asset tools module
	{
		TSharedPtr<IAssetTypeActions> outlineActions = MakeShareable(
			new FRB_OutlineAssetTypeActions(rooibosCategory, subCategories));
		assetTools.RegisterAssetTypeActions(outlineActions.ToSharedRef());
		createdAssetTypeActions.Add(outlineActions);
	}

	// Register custom thumbnail renderer
	UThumbnailManager::Get().RegisterCustomRenderer(URB_OutlineStyle::StaticClass(), URB_OutlineThumbnailRenderer::StaticClass());

	// Enable depth stencils
	URendererSettings* renderSettings = GetMutableDefault<URendererSettings>();
	if (renderSettings != nullptr && renderSettings->CustomDepthStencil != ECustomDepthStencil::EnabledWithStencil)
	{
		renderSettings->CustomDepthStencil = ECustomDepthStencil::EnabledWithStencil;
		renderSettings->SaveConfig(CPF_Config, *renderSettings->GetDefaultConfigFilename());
		FName stencilPropertyName = GET_MEMBER_NAME_CHECKED(URendererSettings, CustomDepthStencil);
		FProperty* stencilProperty = URendererSettings::StaticClass()->FindPropertyByName(stencilPropertyName);
		FPropertyChangedEvent propertyChangedEvent(stencilProperty);
		renderSettings->PostEditChangeProperty(propertyChangedEvent);
	}

	// Set default outline manager class
	URB_OutlineSettings* outlineSettings = GetMutableDefault<URB_OutlineSettings>();
	if (outlineSettings != nullptr && outlineSettings->outlineManagerClass.IsNull())
	{
		outlineSettings->outlineManagerClass = FSoftObjectPath(TEXT("/RooibosOutlines/RB_OutlineManagerBP.RB_OutlineManagerBP_C"));
		outlineSettings->SaveConfig(CPF_Config, *outlineSettings->GetDefaultConfigFilename());
	}
}

void FRooibosOutlinesEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 i = 0; i < createdAssetTypeActions.Num(); ++i)
		{
			AssetTools.UnregisterAssetTypeActions(createdAssetTypeActions[i].ToSharedRef());
		}
	}
	createdAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRooibosOutlinesEditorModule, RooibosOutlinesEditor)