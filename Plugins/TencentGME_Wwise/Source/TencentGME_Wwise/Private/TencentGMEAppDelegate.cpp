#include "TencentGMEAppDelegate.h"

TencentGMEAppDelegate& TencentGMEAppDelegate::GetInstance()
{
    static TencentGMEAppDelegate instance;
    return instance;
}



TencentGMEAppDelegate::TencentGMEAppDelegate()
{
}

#if !defined(__APPLE__)
#include "Core.h"
#include "TencentGMEDevice.h"

static void OnAppDidEnterBackground()
{
	FTencentGMEDevice::StopRecording();
	FTencentGMEDevice::GMEPause();
}

static void OnAppWillEnterForeground()
{
	FTencentGMEDevice::GMEResume();
}

void TencentGMEAppDelegate::Initialize()
{
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddStatic(&OnAppDidEnterBackground);
	FCoreDelegates::ApplicationHasReactivatedDelegate.AddStatic(&OnAppWillEnterForeground);
}
#endif

