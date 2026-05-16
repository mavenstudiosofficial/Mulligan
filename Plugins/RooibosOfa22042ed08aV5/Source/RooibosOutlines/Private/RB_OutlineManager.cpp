// Copyright Joram Wessels 2023


#include "RB_OutlineManager.h"

#include "RB_OutlineSettings.h"
#include "RB_OutlineStyle.h"
#include "RooibosOutlines.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "engine/World.h"

TWeakObjectPtr<ARB_OutlineManager> ARB_OutlineManager::outlineManager = nullptr;

ARB_OutlineManager::ARB_OutlineManager()
{
	PrimaryActorTick.bCanEverTick = false;

	postProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process Component"));
	if (postProcessComponent != nullptr)
	{
		postProcessComponent->bUnbound = true;
	}
	int maxConcurrentOutlines = 0;
	const URB_OutlineSettings* outlineSettings  = GetDefault<URB_OutlineSettings>();
	if (outlineSettings != nullptr)
	{
		maxConcurrentOutlines = outlineSettings->maxConcurrentOutlines;
	}
	loadedOutlines.Init(nullptr, maxConcurrentOutlines);
}

void ARB_OutlineManager::AddOutlineDependency(URB_OutlineComponent* component, URB_OutlineStyle* outline)
{
	if (component == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add outline dependency to outline manager because the given component is null"));
		return;
	}
	if (outline == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add outline dependency to outline manager because the given outline style is null"));
		return;
	}
	FRB_OutlineDependencies* dependentComponents = dependencies.Find(outline);
	if (dependentComponents == nullptr)
	{
		FRB_OutlineDependencies newDependencyList;
		newDependencyList.usedBy.Add(component);
		dependencies.Add(outline, newDependencyList);
		LoadOutlineStyle(outline);
	}
	else
	{
		dependentComponents->usedBy.Add(component);
	}
}

void ARB_OutlineManager::RemoveOutlineDependency(URB_OutlineComponent* component, URB_OutlineStyle* outline)
{
	if (component == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to remove outline dependency to outline manager because the given component is null"));
		return;
	}
	if (outline == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to remove outline dependency to outline manager because the given outline style is null"));
		return;
	}
	FRB_OutlineDependencies* dependentComponents = dependencies.Find(outline);
	if (dependentComponents != nullptr)
	{
		dependentComponents->usedBy.Remove(component);
		if (dependentComponents->usedBy.IsEmpty())
		{
			dependencies.Remove(outline);
			UnloadOutlineStyle(outline);
		}
	}
}

int ARB_OutlineManager::GetStencilValueOfOutline(URB_OutlineStyle* outline) const
{
	if (outline == nullptr)
	{
		return 0;
	}
	const URB_OutlineSettings* settings = GetDefault<URB_OutlineSettings>();
	if (settings == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to get stencil value of outline '%s' because the outline settings CDO is null"),
			*outline->GetName());
		return 0;
	}
	const int index = loadedOutlines.Find(outline);
	if ( ! loadedOutlines.IsValidIndex(index))
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to get stencil value of outline '%s' because the outline style is not loaded into the material"),
			*outline->GetName());
		return 0;
	}
	return settings->firstStencilValue + (1 << index) - 1; // -1 because it's including the first stencil value
}

int ARB_OutlineManager::GetStencilValueForActiveOutlines(const TSet<URB_OutlineStyle*>& activeOutlines) const
{
	const URB_OutlineSettings* settings = GetDefault<URB_OutlineSettings>();
	if (settings == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to get stencil value because the outline settings CDO is null"));
		return 0;
	}
	int stencilValue = 0;
	for (URB_OutlineStyle* outline : activeOutlines)
	{
		const int index = loadedOutlines.Find(outline);
		if ( ! loadedOutlines.IsValidIndex(index))
		{
			UE_LOG(LogRooibosOutlines, Error,
				TEXT("Failed to get stencil value of outline '%s' because the outline style is not loaded into the material"),
				*outline->GetName());
			continue;
		}
		stencilValue |= (1 << index);
	}
	return settings->firstStencilValue + stencilValue - 1; // -1 because it's including the first stencil value
}

const TWeakObjectPtr<ARB_OutlineManager>& ARB_OutlineManager::GetOutlineManager(const UObject* worldcontextObject)
{
	if (outlineManager.IsValid()) // World owns the actor and can therefore destroy it
	{
		return outlineManager;
	}
	outlineManager = nullptr;
	
	if (worldcontextObject == nullptr || worldcontextObject->GetWorld() == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to spawn outline manager because the given world context object '%s' has no world"),
			(worldcontextObject ? *worldcontextObject->GetName() : TEXT("NULL")));
		return outlineManager;
	}
	const URB_OutlineSettings* settings = GetDefault<URB_OutlineSettings>();
	if (settings == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to spawn outline manager because the outline settings CDO is null"));
		return outlineManager;
	}
	if (settings->outlineManagerClass.IsNull())
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to spawn outline manager because no class was specified in the project settings"));
		return outlineManager;
	}
	const TSubclassOf<ARB_OutlineManager> managerClass = settings->outlineManagerClass.LoadSynchronous();
	if (managerClass == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to spawn outline manager of class '%s' because the class could not be loaded"),
			*settings->outlineManagerClass->GetName());
		return outlineManager;
	}
	outlineManager = Cast<ARB_OutlineManager>(worldcontextObject->GetWorld()->SpawnActor(managerClass));

	return outlineManager;
}

void ARB_OutlineManager::BeginPlay()
{
	Super::BeginPlay();

	if (outlineMaterial == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to initialize outline manager because no outline material was specified"));
		return;
	}
	if (postProcessComponent != nullptr)
	{
		dynamicMaterial = UMaterialInstanceDynamic::Create(outlineMaterial, GetWorld(), FName(TEXT("OutlineMaterial")));
		postProcessComponent->Settings.WeightedBlendables.Array.Init(FWeightedBlendable(), 1);
		postProcessComponent->Settings.WeightedBlendables.Array[0].Object = dynamicMaterial;
		postProcessComponent->Settings.WeightedBlendables.Array[0].Weight = 1.0f;
	}
	if (dynamicMaterial != nullptr)
	{
		const URB_OutlineSettings* settings = GetDefault<URB_OutlineSettings>();
		if (settings == nullptr)
		{
			UE_LOG(LogRooibosOutlines, Error,
				TEXT("Failed to set first stencil value because the outline settings CDO is null"));
		}
		else
		{
			dynamicMaterial->SetScalarParameterValue(TEXT("FirstStencilValue"), settings->firstStencilValue);
			dynamicMaterial->SetScalarParameterValue(TEXT("LastStencilValue"), settings->lastStencilValue);
			dynamicMaterial->SetScalarParameterValue(TEXT("LineThickness"), settings->lineThickness);
		}
	}
}

void ARB_OutlineManager::UpdateComponentDependencies_Implementation(URB_OutlineComponent* component,
	const TSet<URB_OutlineStyle*>& previouslyActiveOutlines, const TSet<URB_OutlineStyle*>& activeOutlines)
{
	for (URB_OutlineStyle* previousOutline : previouslyActiveOutlines)
	{
		if ( ! activeOutlines.Contains(previousOutline))
		{
			RemoveOutlineDependency(component, previousOutline);
		}
	}
	for (URB_OutlineStyle* activeOutline : activeOutlines)
	{
		if ( ! previouslyActiveOutlines.Contains(activeOutline))
		{
			AddOutlineDependency(component, activeOutline);
		}
	}
}

void ARB_OutlineManager::LoadOutlineStyle(URB_OutlineStyle* outlineStyle)
{
	if (outlineStyle == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to load outline into material because the given outline is null"));
		return;
	}
	int slotIndex = -1;
	for (int i=0; i<loadedOutlines.Num(); i++)
	{
		if (loadedOutlines[i] == nullptr)
		{
			slotIndex = i;
			break;
		}
	}
	if ( ! loadedOutlines.IsValidIndex(slotIndex))
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to load outline '%s' into material because all outlines lots are occupied"),
			*outlineStyle->GetName());
		return;
	}
	
	loadedOutlines[slotIndex] = outlineStyle;
	LoadOutlineStyleIntoMaterialSlot(outlineStyle, slotIndex);
}

void ARB_OutlineManager::UnloadOutlineStyle(URB_OutlineStyle* outlineStyle)
{
	if (outlineStyle == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to unload outline from material because the given outline is null"));
		return;
	}
	const int slotIndex = loadedOutlines.Find(outlineStyle);
	if (loadedOutlines.IsValidIndex(slotIndex))
	{
		loadedOutlines[slotIndex] = nullptr;
	}
	LoadOutlineStyleIntoMaterialSlot(nullptr, slotIndex);
}

void ARB_OutlineManager::LoadOutlineStyleIntoMaterialSlot(const URB_OutlineStyle* outlineStyle, int slotIndex) const
{
	if (dynamicMaterial == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to load outline style '%s' into material slot %d because the post process material is not dynamic"),
			(outlineStyle ? *outlineStyle->GetName() : TEXT("NULL")), slotIndex);
		return;
	}
	if (outlineStyle != nullptr)
	{
		UTexture2D* fillTexture = (outlineStyle->fillTexture ? outlineStyle->fillTexture : nullTexture);
		dynamicMaterial->SetVectorParameterValue(FName(FString::Printf(TEXT("Color_%d"), slotIndex)), outlineStyle->color);
		dynamicMaterial->SetTextureParameterValue(FName(FString::Printf(TEXT("FillTexture_%d"), slotIndex)), fillTexture);
		dynamicMaterial->SetScalarParameterValue(FName(FString::Printf(TEXT("TextureTiling_%d"), slotIndex)), outlineStyle->textureTiling);
		dynamicMaterial->SetScalarParameterValue(FName(FString::Printf(TEXT("TextureOpacity_%d"), slotIndex)), outlineStyle->textureOpacity);
		dynamicMaterial->SetScalarParameterValue(FName(FString::Printf(TEXT("PulseIntensity_%d"), slotIndex)), outlineStyle->pulseIntensity);
		dynamicMaterial->SetScalarParameterValue(FName(FString::Printf(TEXT("PulseFrequency_%d"), slotIndex)), outlineStyle->pulseFrequency);
	}
	else
	{
		dynamicMaterial->SetTextureParameterValue(FName(FString::Printf(TEXT("FillTexture_%d"), slotIndex)), nullTexture);
	}
}

