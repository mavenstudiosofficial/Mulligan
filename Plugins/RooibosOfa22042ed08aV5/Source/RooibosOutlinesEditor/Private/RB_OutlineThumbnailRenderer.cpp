// Copyright Joram Wessels 2023


#include "RB_OutlineThumbnailRenderer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RB_OutlineStyle.h"
#include "ThumbnailRendering/ThumbnailManager.h"

void URB_OutlineThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth,
                                                    uint32& OutHeight) const
{
	URB_OutlineStyle* outline = Cast<URB_OutlineStyle>(Object);
	if (outline == nullptr)
	{
		return;
	}
	Super::GetThumbnailSize(outline->fillTexture, Zoom, OutWidth, OutHeight);
}

void URB_OutlineThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height,
	FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	URB_OutlineStyle* outline = Cast<URB_OutlineStyle>(Object);
	if (outline == nullptr)
	{
		return;
	}

	float outlineWidth = floor(Width * outlinePercentageWidth);
	
	// If using alpha, draw a checkerboard underneath first.
	const int32 CheckerDensity = 8;
	auto Checker = UThumbnailManager::Get().CheckerboardTexture;
	Canvas->DrawTile(
		0.0f, 0.0f, Width, Height,							// Dimensions
		0.0f, 0.0f, CheckerDensity, CheckerDensity,			// UVs
		FLinearColor::White, Checker->GetResource());			// Tint & Texture

	// Drawing texture
	if (outline->fillTexture != nullptr && outline->fillTexture->GetResource() != nullptr)
	{
		FVector2D textureStart = FVector2D(outlineWidth / 2, outlineWidth / 2);
		FVector2D textureEnd = FVector2D(Width - outlineWidth, Height - outlineWidth);
		FLinearColor textureColor = outline->color;
		textureColor.A *= outline->textureOpacity;
		FCanvasTileItem CanvasTile( textureStart, outline->fillTexture->GetResource(), textureEnd, textureColor );
		CanvasTile.BlendMode = SE_BLEND_Translucent;
		if (outline->fillTexture->Source.GetNumBlocks() > 1)
		{
			// Adjust UVs to display entire UDIM range, acounting for UE inverted V-axis
			// We're not actually rendering a VT here, but the editor-only texture we're using is still using the UDIM tile layout
			// So we use inverted Y-axis, but then normalize back to [0,1)
			const FIntPoint BlockSize = outline->fillTexture->Source.GetSizeInBlocks();
			const float RcpBlockSizeY = 1.0f / (float)BlockSize.Y;
			CanvasTile.UV0.Y = (1.0f - (float)BlockSize.Y) * RcpBlockSizeY;
			CanvasTile.UV1.Y = RcpBlockSizeY;
		}
		CanvasTile.Draw( Canvas );
	}

	// Drawing outline (TODO: Line drawing doesn't seem to support translucency. Could try border/box drawing instead)
	FCanvasLineItem line;
	line.LineThickness = outlineWidth;
	line.BlendMode = SE_BLEND_AlphaBlend;
	line.SetColor(outline->color);
	line.Origin = FVector(X, Y, 0);
	line.EndPos = FVector(Width, Y, 0);
	Canvas->DrawItem(line);
	line.Origin = FVector(Width, Y, 0);
	line.EndPos = FVector(Width, Height, 0);
	Canvas->DrawItem(line);
	line.Origin = FVector(Width, Height, 0);
	line.EndPos = FVector(X, Height, 0);
	Canvas->DrawItem(line);
	line.Origin = FVector(X, Height, 0);
	line.EndPos = FVector(X, Y, 0);
	Canvas->DrawItem(line);
	
}
