// Copyright Joram Wessels 2023

#include "RooibosOutlines.h"

#define LOCTEXT_NAMESPACE "FRooibosOutlinesModule"

DEFINE_LOG_CATEGORY(LogRooibosOutlines);

void FRooibosOutlinesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRooibosOutlinesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRooibosOutlinesModule, RooibosOutlines)

