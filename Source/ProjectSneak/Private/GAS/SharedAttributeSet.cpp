// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SharedAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayEffectExtension.h"


USharedAttributeSet::USharedAttributeSet()
{
	Health = 2.f;
	MaxHealth = 2.f;
	Stamina = 100.f;
	MaxStamina = 100.f;
	SpeedMultiplier = 1.0f;
	Shield = 0.0f;
	MaxShield = 10.0f;
}

void USharedAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, SpeedMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USharedAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
}

void USharedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetShieldAttribute() && IsValid(GetOwningAbilitySystemComponent()))
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

		const float NewShield = Shield.GetCurrentValue();
		float Damage = Data.EvaluatedData.Magnitude;

		FGameplayTag ShieldTag = FGameplayTag::RequestGameplayTag(FName("Data.Shield"));
		FGameplayTagContainer ShieldTagContainer;
		ShieldTagContainer.AddTag(ShieldTag);
		TArray< FActiveGameplayEffectHandle> ShieldHandles = ASC->GetActiveEffectsWithAllTags(ShieldTagContainer);

		if (Damage < 0 && !ShieldHandles.IsEmpty()) // Shield has lowered but is still active
		{
			for(FActiveGameplayEffectHandle Handle : ShieldHandles)
			{
				const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle);
				float ThisShieldValue = ActiveGE->Spec.GetSetByCallerMagnitude(ShieldTag, false);
				if (FMath::Abs(Damage) >= ThisShieldValue) 
				{
					Damage += ThisShieldValue;
					ASC->RemoveActiveGameplayEffect(Handle);
					SetShield(0);
				}
				else
				{
					TMap<FGameplayTag, float> NewMagnitudes;
					NewMagnitudes.Add(ShieldTag, ThisShieldValue + Damage);
					ASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(Handle, NewMagnitudes);
				}

				if (Damage >= 0)
				{
					break;
				}
			}
		}
		else if (Damage < 0) 
		{
			OnHealthDamaged.Broadcast(Damage);

			for (FActiveGameplayEffectHandle Handle : ShieldHandles)
			{
				ASC->RemoveActiveGameplayEffect(Handle);
			}
			SetShield(0);
		}
	}
}

void USharedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}

	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}
