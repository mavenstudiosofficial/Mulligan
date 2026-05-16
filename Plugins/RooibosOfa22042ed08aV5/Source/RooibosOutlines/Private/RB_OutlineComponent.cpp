// Copyright Joram Wessels 2023


#include "RB_OutlineComponent.h"

#include "RB_OutlineManager.h"
#include "RB_OutlineSettings.h"
#include "RB_OutlineStyle.h"
#include "RooibosOutlines.h"
#include "Engine/World.h"
#include "Components/MeshComponent.h"

URB_OutlineComponent::URB_OutlineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URB_OutlineComponent::SetComponentsToApplyOutlineOn(TSet<UMeshComponent*> components)
{
	TSet<URB_OutlineStyle*> currentOutlines = activeOutlines;
	for (URB_OutlineStyle* outline : currentOutlines)
	{
		SetOutlineEnabled(false, outline);
	}

	componentsToApplyOutlineOn = components;

	for (URB_OutlineStyle* outline : currentOutlines)
	{
		SetOutlineEnabled(true, outline);
	}
}

void URB_OutlineComponent::SetOutlineEnabled(bool enabled, URB_OutlineStyle* outline)
{
	if (outline == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error, TEXT("Failed to %s outline on actor '%s' because no outline style was provided"),
			(enabled ? TEXT("enable") : TEXT("disable")), *AActor::GetDebugName(GetOwner()));
		return;
	}
	if ((enabled && activeOutlines.Contains(outline)) ||
		( ! enabled && ! activeOutlines.Contains(outline)))
	{
		UE_LOG(LogRooibosOutlines, Log,
			TEXT("Attempted to %s outline '%s' on actor '%s' but it was already %s"),
			(enabled ? TEXT("enable") : TEXT("disable")), *outline->GetName(), *AActor::GetDebugName(GetOwner()),
			(enabled ? TEXT("enabled") : TEXT("disabled")));
		return;
	}
	
	if (enabled)
	{
		activeOutlines.Add(outline);
	}
	else
	{
		activeOutlines.Remove(outline);
	}

	UpdateStencilValue();
}

void URB_OutlineComponent::SetOutlineEnabledForActor(AActor* actor, bool enabled, URB_OutlineStyle* outline)
{
	if (actor == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error, TEXT("Failed to %s outline on actor because the given actor is null"),
			(enabled ? TEXT("enable") : TEXT("disable")));
		return;
	}
	URB_OutlineComponent* outlineComponent = actor->FindComponentByClass<URB_OutlineComponent>();
	if (outlineComponent == nullptr)
	{
		outlineComponent = AddOutlineComponentToActor(actor);
		if (outlineComponent == nullptr)
		{
			UE_LOG(LogRooibosOutlines, Error,
				TEXT("Failed to %s outline on actor '%s' because the outline component failed to instantiate"),
				(enabled ? TEXT("enable") : TEXT("disable")), *AActor::GetDebugName(actor));
			return;
		}
	}
	outlineComponent->SetOutlineEnabled(enabled, outline);
}

URB_OutlineComponent* URB_OutlineComponent::AddOutlineComponentToActor(AActor* actor)
{
	const URB_OutlineSettings* settings = GetDefault<URB_OutlineSettings>();
	if (settings == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add outline component to actor '%s' because the outline settings CDO is null"),
			*AActor::GetDebugName(actor));
		return nullptr;
	}
	if (settings->outlineComponentClass.IsNull())
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add outline component to actor '%s' because no class was specified in the project settings"),
			*AActor::GetDebugName(actor));
		return nullptr;
	}
	const TSubclassOf<URB_OutlineComponent> componentClass = settings->outlineComponentClass.LoadSynchronous();
	if (componentClass == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add '%s' to actor '%s' because the outline component class could not be loaded"),
			*settings->outlineComponentClass->GetName(), *AActor::GetDebugName(actor));
		return nullptr;
	}
	URB_OutlineComponent* outlineComponent = NewObject<URB_OutlineComponent>(actor, componentClass);
	if (outlineComponent == nullptr)
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to add '%s' to actor '%s' because the outline component class failed to instantiate"),
			*settings->outlineComponentClass->GetName(), *AActor::GetDebugName(actor));
		return nullptr;
	}
	outlineComponent->RegisterComponent();
	return outlineComponent;
}

void URB_OutlineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (componentsToApplyOutlineOn.IsEmpty())
	{
		if (GetOwner() == nullptr)
		{
			return;
		}
		TArray<UMeshComponent*> meshes;
		GetOwner()->GetComponents(meshes);
		for (UMeshComponent* mesh : meshes)
		{
			if (mesh == nullptr)
			{
				continue;
			}
			componentsToApplyOutlineOn.Add(mesh);
		}
	}
	if ( ! activeOutlines.IsEmpty())
	{
		UpdateStencilValue();
	}
}

void URB_OutlineComponent::UpdateStencilValue()
{
	if (GetWorld() == nullptr || (GetWorld()->IsEditorWorld() && ! GetWorld()->IsPlayInEditor()))
	{
		return;
	}
	const TWeakObjectPtr<ARB_OutlineManager>& manager = ARB_OutlineManager::GetOutlineManager(this);
	if ( ! manager.IsValid())
	{
		UE_LOG(LogRooibosOutlines, Error,
			TEXT("Failed to update outline on actor '%s' because the object manager could not be spawned"),
			*AActor::GetDebugName(GetOwner()));
		return;
	}

	manager->UpdateComponentDependencies(this, previousActiveOutlines, activeOutlines);
	const int stencilValue = manager->GetStencilValueForActiveOutlines(activeOutlines);

	for (UMeshComponent* mesh : componentsToApplyOutlineOn)
	{
		if (mesh == nullptr)
		{
			continue;
		}
		mesh->SetRenderCustomDepth(stencilValue != 0);
		mesh->SetCustomDepthStencilValue(stencilValue);
	}

	previousActiveOutlines = activeOutlines;
}

#if WITH_EDITORONLY_DATA
void URB_OutlineComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(URB_OutlineComponent, activeOutlines))
	{
		UpdateStencilValue();
	}
}
#endif
