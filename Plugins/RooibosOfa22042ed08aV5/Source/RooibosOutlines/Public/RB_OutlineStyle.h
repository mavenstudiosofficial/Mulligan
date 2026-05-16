// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "RB_OutlineStyle.generated.h"

/**
 * The appearance of an outline
 */
UCLASS(BlueprintType)
class ROOIBOSOUTLINES_API URB_OutlineStyle : public UDataAsset
{
	GENERATED_BODY()

public:

	/** The color of the outline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	FLinearColor color;

	/** A monochromatic texture to use for the filling. Will be rendered using the outline color. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	UTexture2D* fillTexture = nullptr;

	/** The multiplier for the texture tiling. Higher means more tiles / smaller texture. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines", meta = (ClampMin = 0.00001f, ClampMax = 100, EditCondition = fillTexture))
	float textureTiling = 1.0f;

	/** The opacity of the fill texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines", meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = fillTexture))
	float textureOpacity = 1.0f;

	/** How much the outline fades during pulsing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float pulseIntensity = 0.0f;

	/** How quickly the outline pulses / flickers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines", meta = (ClampMin = 0.001f, EditCondition = "pulseIntensity>0.0f"))
	float pulseFrequency = 0.0f;
	
};
