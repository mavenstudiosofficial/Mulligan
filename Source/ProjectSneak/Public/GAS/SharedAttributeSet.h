// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectComponent.h"
#include "GameplayEffectExtension.h"
#include "SharedAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatChanged, float, NewValue);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class PROJECTSNEAK_API USharedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USharedAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, MaxHealth)


	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, MaxStamina)


	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpeedMultiplier)
	FGameplayAttributeData SpeedMultiplier;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, SpeedMultiplier)


	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, Shield)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(USharedAttributeSet, MaxShield)

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, Health, OldValue);
	}

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, MaxHealth, OldValue);
	}

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, Stamina, OldValue);
	}

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, MaxStamina, OldValue);
	}

	UFUNCTION()
	void OnRep_SpeedMultiplier(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, SpeedMultiplier, OldValue);
	}

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, Shield, OldValue);
	}

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldValue)const {
		GAMEPLAYATTRIBUTE_REPNOTIFY(USharedAttributeSet, MaxShield, OldValue);
	}

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnFloatChanged OnHealthDamaged;
};
