// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include <Net/UnrealNetwork.h>
#include "MGFImplementationAbilityAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(Abstract, EditInlineNew)
class MGFIMPLEMENTATION_API UMGFImplementationAbilityAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMGFImplementationAbilityAttributeSet() {}

	UPROPERTY(BlueprintReadWrite, Category = "Health", ReplicatedUsing = "OnRep_Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMGFImplementationAbilityAttributeSet, Health)

	UPROPERTY(BlueprintReadWrite, Category = "Stamina", ReplicatedUsing = "OnRep_Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMGFImplementationAbilityAttributeSet, Stamina)

	UPROPERTY(BlueprintReadWrite, Category = "Special", ReplicatedUsing = "OnRep_Special")
	FGameplayAttributeData Special;
	ATTRIBUTE_ACCESSORS(UMGFImplementationAbilityAttributeSet, Special)


	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth) {
		
		GAMEPLAYATTRIBUTE_REPNOTIFY(UMGFImplementationAbilityAttributeSet, Health, OldHealth);
	}
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina) {

		GAMEPLAYATTRIBUTE_REPNOTIFY(UMGFImplementationAbilityAttributeSet, Stamina, OldStamina);
	}
	UFUNCTION()
	virtual void OnRep_Special(const FGameplayAttributeData& OldSpecial) {
		
		GAMEPLAYATTRIBUTE_REPNOTIFY(UMGFImplementationAbilityAttributeSet, Special, OldSpecial);
	}


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME_CONDITION_NOTIFY(UMGFImplementationAbilityAttributeSet, Health, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UMGFImplementationAbilityAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UMGFImplementationAbilityAttributeSet, Special, COND_None, REPNOTIFY_Always);
	}
};
