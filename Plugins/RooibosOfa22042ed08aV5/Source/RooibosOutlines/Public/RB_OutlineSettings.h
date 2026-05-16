// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RB_OutlineSettings.generated.h"

/**
 * The developer settings for the Rooibos Outline plugin
 */
UCLASS(DefaultConfig, Config = Game, DisplayName = "Rooibos Outline Settings")
class ROOIBOSOUTLINES_API URB_OutlineSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	URB_OutlineSettings();

	/**
	 * The first stencil value used by the outline system.
	 * Change this if you want to use depth stencils for other purposes.
	 * @note This influences the maximum number of concurrent outlines
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines|Stencils", meta = (ClampMin = 1, ClampMax = 255))
	int firstStencilValue = 1;

	/**
	 * The last stencil value used by the outline system.
	 * Change this if you want to use depth stencils for other purposes.
	 * @note This influences the maximum number of concurrent outlines
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines|Stencils", meta = (ClampMin = 1, ClampMax = 255))
	int lastStencilValue = 255;

	/** The maximum number of outlines that can be represented; depends on the range of stencil values available */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines|Stencils")
	int maxConcurrentOutlines = 8;

	/** The width of the outlines (these can't be set on a per outline basis because math) */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines", meta = (ClampMin = 1, ClampMax = 10))
	float lineThickness = 4.0f;

	/** The manager class to spawn once the first actor has an outline */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	TSoftClassPtr<class ARB_OutlineManager> outlineManagerClass = nullptr;

	/** The component class to add to an actor when its first outline channel is enabled */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	TSoftClassPtr<class URB_OutlineComponent> outlineComponentClass = nullptr;

protected:

	void CalculateMaxConcurrentOutlines();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
