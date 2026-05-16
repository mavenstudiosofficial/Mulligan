#pragma once

#include "CoreMinimal.h"
#include "IGameSystem.h"
#include "UGameSystemBase.generated.h"
//#include "games"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PROJECTSNEAK_API UGameSystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UGameSystemBase();
	virtual ~UGameSystemBase() override;
};


