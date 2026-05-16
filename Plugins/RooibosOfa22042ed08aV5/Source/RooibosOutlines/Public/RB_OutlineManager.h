// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RB_OutlineManager.generated.h"

/**
 * Struct to track which outline components are expecting an outline style to be available in the post process material
 */
USTRUCT()
struct FRB_OutlineDependencies
{
	GENERATED_BODY()

	/** The outline components currently dependent on this outline */
	UPROPERTY()
	TSet<class URB_OutlineComponent*> usedBy;
};

UCLASS(PrioritizeCategories = "Rooibos Outlines")
class ROOIBOSOUTLINES_API ARB_OutlineManager : public AActor
{
	GENERATED_BODY()

public:
	
	ARB_OutlineManager();

	/** Lets the manager know which outlines this component needs to be loaded into the material */
	UFUNCTION(BlueprintNativeEvent, Category = "Rooibos Outlines")
	void UpdateComponentDependencies(class URB_OutlineComponent* component,
		const TSet<class URB_OutlineStyle*>& previouslyActiveOutlines, const TSet<class URB_OutlineStyle*>& activeOutlines);

	/** Lets the manager know that the given component requires the given outline to be loaded into the material */
	void AddOutlineDependency(class URB_OutlineComponent* component, class URB_OutlineStyle* outline);
	
	/** Lets the manager know that the given component no longer requires the given outline to be loaded into the material */
	void RemoveOutlineDependency(class URB_OutlineComponent* component, class URB_OutlineStyle* outline);

	/** Returns the stencil value to enable in order to show the given outline around an actor */
	int GetStencilValueOfOutline(class URB_OutlineStyle* outline) const;

	/** Returns the stencil value to enable in order to show the given outlines around an actor */
	int GetStencilValueForActiveOutlines(const TSet<class URB_OutlineStyle*>& activeOutlines) const;

	static const TWeakObjectPtr<ARB_OutlineManager>& GetOutlineManager(const UObject* worldcontextObject);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Rooibos Outlines")
	class UPostProcessComponent* postProcessComponent = nullptr;

	/** The post processing material shading the custom depth stencils */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	class UMaterialInterface* outlineMaterial = nullptr;

	/** The texture to use to unload textures (because texture material params can't be set to null) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rooibos Outlines")
	class UTexture2D* nullTexture = nullptr;

	/** The material whose variables can be changed during runtime */
	UPROPERTY(BlueprintReadWrite, Category = "Rooibos Outlines")
	class UMaterialInstanceDynamic* dynamicMaterial = nullptr;

	/** The outline styles that need to be loaded into the post process material, and the components using them */
	UPROPERTY(VisibleInstanceOnly, Category = "Rooibos Outlines")
	TMap<class URB_OutlineStyle*, FRB_OutlineDependencies> dependencies;

	/** Which outline slots in the material are already occupied */
	UPROPERTY(VisibleInstanceOnly, Category = "Rooibos Outlines")
	TArray<class URB_OutlineStyle*> loadedOutlines;

	virtual void BeginPlay() override;
	
	/** Lets the manager know which outlines this component needs to be loaded into the material */
	virtual void UpdateComponentDependencies_Implementation(class URB_OutlineComponent* component,
		const TSet<class URB_OutlineStyle*>& previouslyActiveOutlines, const TSet<class URB_OutlineStyle*>& activeOutlines);

	void LoadOutlineStyle(class URB_OutlineStyle* outlineStyle);

	void UnloadOutlineStyle(class URB_OutlineStyle* outlineStyle);

	void LoadOutlineStyleIntoMaterialSlot(const class URB_OutlineStyle* outlineStyle, int slotIndex) const;

private:

	static constexpr int bitwiseMaxOutlines = 8;
	static constexpr int indexwiseMaxOutlines = 16;
	
	static TWeakObjectPtr<ARB_OutlineManager> outlineManager;
	
};
