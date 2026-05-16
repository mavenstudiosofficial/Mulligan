// Copyright Joram Wessels 2023

#include "RB_OutlineAssetTypeActions.h"

#include "RB_OutlineStyle.h"

UClass* FRB_OutlineAssetTypeActions::GetSupportedClass() const
{
	return URB_OutlineStyle::StaticClass();
}
