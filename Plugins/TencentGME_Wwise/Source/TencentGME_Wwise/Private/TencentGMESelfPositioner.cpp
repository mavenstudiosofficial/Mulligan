#include "TencentGMESelfPositioner.h"
#include "TencentGMEDevice.h"


UTencentGMESelfPositioner::UTencentGMESelfPositioner(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{}

FVector UTencentGMESelfPositioner::GetPosition() const
{
	return CurrentPosition;
}

void UTencentGMESelfPositioner::BeginPlay()
{
	Super::BeginPlay();

	SetSelfPosition(GetRoundedTransformPosition());
}

void UTencentGMESelfPositioner::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	auto newPosition = GetRoundedTransformPosition();
	if (FVector::DistSquared(CurrentPosition, newPosition) >= MinimumDeltaPosition * MinimumDeltaPosition)
		SetSelfPosition(newPosition);
}

FVector UTencentGMESelfPositioner::GetRoundedTransformPosition() const
{
	auto vector = GetComponentTransform().GetTranslation();
	return { FMath::RoundToFloat(vector.X), FMath::RoundToFloat(vector.Y), FMath::RoundToFloat(vector.Z) };
}

void UTencentGMESelfPositioner::SetSelfPosition(const FVector& vector)
{
	CurrentPosition = vector;
	FTencentGMEDevice::SetSelfPosition(static_cast<int>(CurrentPosition.X), static_cast<int>(CurrentPosition.Y), static_cast<int>(CurrentPosition.Z));
	//UE_LOG(LogTencentGME, Log, TEXT("Set Self Position: (%d, %d, %d)"), static_cast<int>(CurrentPosition.X), static_cast<int>(CurrentPosition.Y), static_cast<int>(CurrentPosition.Z));
}
