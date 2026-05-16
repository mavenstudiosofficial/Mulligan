// Copyright Joram Wessels 2023


#include "RB_OutlineSettings.h"

#include "RB_OutlineComponent.h"

URB_OutlineSettings::URB_OutlineSettings()
{
	outlineComponentClass = URB_OutlineComponent::StaticClass();
}

void URB_OutlineSettings::CalculateMaxConcurrentOutlines()
{
	const int availableStencils = lastStencilValue - firstStencilValue + 1 + 1; // +1 because it's including the values themselves
	const int roundedUp = FMath::RoundUpToPowerOfTwo(availableStencils); // another +1 because we always have a stencil for all outlines disabled
	const bool valueWasBelowRoundedPowerOf2 = availableStencils < roundedUp;
	const int roundedDown = (valueWasBelowRoundedPowerOf2 ? roundedUp / 2 : roundedUp);
	maxConcurrentOutlines = FMath::LogX(2, static_cast<float>(roundedDown));
}

#if WITH_EDITOR
void URB_OutlineSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(URB_OutlineSettings, firstStencilValue) ||
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(URB_OutlineSettings, lastStencilValue))
	{
		if (lastStencilValue < firstStencilValue)
		{
			lastStencilValue = firstStencilValue;
		}
		CalculateMaxConcurrentOutlines();
	}
}
#endif
