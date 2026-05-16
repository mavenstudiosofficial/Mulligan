// Copyright Joram Wessels 2023


#include "RB_OutlineFactory.h"

#include "RB_OutlineStyle.h"

URB_OutlineFactory::URB_OutlineFactory()
{
	SupportedClass = URB_OutlineStyle::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* URB_OutlineFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	URB_OutlineStyle* newOutlineStyle = NewObject<URB_OutlineStyle>(InParent, InClass, InName, Flags | RF_Transactional);
	return newOutlineStyle;
}
