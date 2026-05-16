#include "PSAssetSystemModule.h"

#include "AssetListener.h"

IMPLEMENT_GAME_MODULE(FPSAssetSystemModule, PSAssetSystem);

void FPSAssetSystemModule::StartupModule()
{
    UAssetListener::Startup();
}

void FPSAssetSystemModule::ShutdownModule()
{
    UAssetListener::Shutdown();
}