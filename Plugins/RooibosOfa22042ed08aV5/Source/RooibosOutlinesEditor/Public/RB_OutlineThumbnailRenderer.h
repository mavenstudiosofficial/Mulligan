// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "RB_OutlineThumbnailRenderer.generated.h"

/**
 * 
 */
UCLASS()
class ROOIBOSOUTLINESEDITOR_API URB_OutlineThumbnailRenderer : public UTextureThumbnailRenderer
{
	GENERATED_BODY()
	
public:

	virtual void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily) override;

protected:

	/** The width of the outline in the asset thumbnail as a percentage of the thumbnail size */
	inline static constexpr float outlinePercentageWidth = .1f;
};
