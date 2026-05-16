// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RB_OutlineFactory.generated.h"

/**
 * 
 */
UCLASS()
class ROOIBOSOUTLINESEDITOR_API URB_OutlineFactory : public UFactory
{
	GENERATED_BODY()

public:

	URB_OutlineFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn) override;
	
};
