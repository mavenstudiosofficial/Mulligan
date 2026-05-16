#include "TencentGME_Wwise.h"
#include "TencentGMEDevice.h"
#include "TencentGMEAppDelegate.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/MinimalWindowsApi.h"
#endif
#include <AK/SoundEngine/Common/IAkPlugin.h> 
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#endif

#include <AK/AkWwiseSDKVersion.h>

#define LOCTEXT_NAMESPACE "FTencentGME_WwiseModule"

#if WITH_EDITOR
DEFINE_PLUGIN_REGISTER_HOOK
#endif
#include "AkAudioDevice.h"
#include "AkAudioModule.h"

#ifndef TENCENTGME_WWISE_DYNAMIC
#include <AK/Plugin/TencentGMEFactory.h>
#endif

#ifdef _DEBUG
AkAssertHook g_pAssertHook = NULL;
#endif

void FTencentGME_WwiseModule::StartupModule()
{
#if defined(TENCENTGME_WWISE_DYNAMIC) || (WITH_EDITOR && PLATFORM_MAC)
	auto AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice)
	{
#if WITH_EDITOR && PLATFORM_MAC
		const FString PluginName = TEXT("UE4Editor-TencentGME_Wwise");
		const FString EditorPlatform = TEXT("Mac");
		FString PluginDir = FPaths::Combine(*IPluginManager::Get().FindPlugin("TencentGME_Wwise")->GetBaseDir(), TEXT("Binaries"), EditorPlatform);
#else
		const FString PluginName = TEXT("TencentGME");
		FString PluginDir = FPlatformProcess::BaseDir();
#endif

#if defined(PLATFORM_XBOXONEGDK) && PLATFORM_XBOXONEGDK
		if (PluginDir.IsEmpty())
		{
			PluginDir = FString::Printf(TEXT("%s/Binaries/XboxOneGDK"), FApp::GetProjectName());
		}
#elif defined(PLATFORM_XBOXONE) && PLATFORM_XBOXONE
		if (PluginDir.IsEmpty())
		{
			PluginDir = FString::Printf(TEXT("%s/Binaries/XboxOne"), FApp::GetProjectName());
		}
#elif defined(PLATFORM_XSX) && PLATFORM_XSX
		if (PluginDir.IsEmpty())
		{
			PluginDir = FString::Printf(TEXT("%s/Binaries/XSX"), FApp::GetProjectName());
		}
#endif

		UE_LOG(LogTencentGME, Warning, TEXT("Attempting to load TencentGME plug-in at %s."), *PluginDir);
		auto Result = AkAudioDevice->RegisterPluginDLL(PluginName, PluginDir);

#if WITH_EDITOR && PLATFORM_WINDOWS
		if (Result != AK_Success)
		{
			UE_LOG(LogTencentGME, Warning, TEXT("Failed to load TencentGME plug-in at %s."), *PluginDir);
			const FString EditorPlatform = TEXT("Win64");
			PluginDir = FPaths::Combine(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), TEXT("Binaries"), EditorPlatform);
			UE_LOG(LogTencentGME, Warning, TEXT("Attempting to load TencentGME plug-in at %s."), *PluginDir);

			Result = AkAudioDevice->RegisterPluginDLL(PluginName, PluginDir);
			if (Result != AK_Success)
			{
				UE_LOG(LogTencentGME, Warning, TEXT("Failed to load TencentGME plug-in at %s."), *PluginDir);
				PluginDir = FPaths::Combine(*IPluginManager::Get().FindPlugin("TencentGME_Wwise")->GetBaseDir(), TEXT("Binaries"), EditorPlatform);
				UE_LOG(LogTencentGME, Warning, TEXT("Attempting to load TencentGME plug-in at %s."), *PluginDir);

				Result = AkAudioDevice->RegisterPluginDLL(PluginName, PluginDir);
			}
		}
#endif // WITH_EDITOR && PLATFORM_WINDOWS

		if (Result != AK_Success)
		{
			UE_LOG(LogTencentGME, Error, TEXT("Failed to load TencentGME plug-in at %s. TencentGME plug-in was not loaded."), *PluginDir);
		}
		else if (!IAkAudioModule::IsAvailable())
		{
			UE_LOG(LogTencentGME, Error, TEXT("IAkAudioModule is not available."));
		}
		else if (!FAkAudioModule::AkAudioModuleInstance)
		{
			UE_LOG(LogTencentGME, Error, TEXT("FAkAudioModule is NULL."));
		}
		else
		{
			UE_LOG(LogTencentGME, Log, TEXT("Reloading Wwise Asset Data."));
			FAkAudioModule::AkAudioModuleInstance->ReloadWwiseAssetData();
		}
	}
#endif // defined(TENCENTGME_WWISE_DYNAMIC) || (WITH_EDITOR && PLATFORM_MAC)

	TencentGMEAppDelegate::GetInstance().Initialize();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTencentGME_WwiseModule, TencentGME_Wwise)
