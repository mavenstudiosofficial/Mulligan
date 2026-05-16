#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameSystem.generated.h"

UINTERFACE(MinimalAPI)
class UGameSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSNEAK_API IGameSystem
{
	GENERATED_BODY()
public:

	virtual void InitializeSystem() {}
	virtual void ShutdownSystem() {}
};
