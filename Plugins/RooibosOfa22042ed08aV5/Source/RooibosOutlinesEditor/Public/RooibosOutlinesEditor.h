// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRooibosOutlinesEditorModule : public IModuleInterface
{
public:

    TArray<TSharedPtr<class IAssetTypeActions>> createdAssetTypeActions;
    
    virtual void StartupModule() override;
    
    virtual void ShutdownModule() override;
    
};
