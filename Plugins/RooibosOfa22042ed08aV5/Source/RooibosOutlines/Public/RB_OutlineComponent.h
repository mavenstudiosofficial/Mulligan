// Copyright Joram Wessels 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RB_OutlineComponent.generated.h"

/**
 * Keeps track of the outline channels activated on the owning actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROOIBOSOUTLINES_API URB_OutlineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	URB_OutlineComponent();

	/** Specifies what mesh components should be highlighted when an outline is activated on this actor */
	UFUNCTION(BlueprintCallable, Category = "Rooibos Outlines")
	void SetComponentsToApplyOutlineOn(TSet<class UMeshComponent*> components);

	/** Enables or disables the given outline on this actor */
	UFUNCTION(BlueprintCallable, Category = "Rooibos Outlines")
	void SetOutlineEnabled(bool enabled, class URB_OutlineStyle* outline);

	/** Enables or disables the given outline on the given actor */
	UFUNCTION(BlueprintCallable, Category = "Rooibos Outlines")
	static void SetOutlineEnabledForActor(AActor* actor, bool enabled, class URB_OutlineStyle* outline);

	/** Finds or creates the outline component on the given actor */
	static class URB_OutlineComponent* AddOutlineComponentToActor(AActor* actor);

protected:

	/** The outlines currently shown around the owning actor */
	UPROPERTY(EditAnywhere, Category = "Rooibos Outlines")
	TSet<class URB_OutlineStyle*> activeOutlines;

	/** The names of the mesh components to apply the outline effect on */
	UPROPERTY(EditDefaultsOnly, Category = "Rooibos Outlines")
	TSet<class UMeshComponent*> componentsToApplyOutlineOn;
	
	/** The outlines previously shown around the owning actor since the last time the stencil value was updated */
	UPROPERTY()
	TSet<class URB_OutlineStyle*> previousActiveOutlines;

	virtual void BeginPlay() override;

	void UpdateStencilValue();
	
#if WITH_EDITORONLY_DATA
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

};
