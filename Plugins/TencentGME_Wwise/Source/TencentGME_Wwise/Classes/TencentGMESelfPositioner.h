#pragma once

#include "Components/SceneComponent.h"
#include "TencentGMESelfPositioner.generated.h"

/**
* This class automatically updates the position of the local GME user.
*/
UCLASS(ClassGroup = TencentGME, BlueprintType, Blueprintable, hidecategories = (Transform, Rendering, Mobility, LOD, Component, Activation), AutoExpandCategories = TencentGMESelfPositioner, meta = (BlueprintSpawnableComponent))
class TENCENTGME_WWISE_API UTencentGMESelfPositioner : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TencentGMESelfPositioner")
	FVector GetPosition() const;

	/**
	* The minimum change in distance required to automatically trigger an update in position sent to the GME SDK.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TencentGMESelfPositioner", meta = (ClampMin = 1, ToolTip = "The minimum change in distance required to automatically trigger an update in position sent to the GME SDK."))
	int MinimumDeltaPosition = 1;

#if CPP
public:
	virtual void BeginPlay() override;
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

private:
	FVector CurrentPosition;

	FVector GetRoundedTransformPosition() const;
	void SetSelfPosition(const FVector& vector);
#endif
};
