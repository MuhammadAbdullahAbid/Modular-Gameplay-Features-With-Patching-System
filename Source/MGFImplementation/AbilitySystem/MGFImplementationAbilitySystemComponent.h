// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "MGFImplementationAbilitySystemComponent.generated.h"

class UAbilityAnimNotify;
class USkeletalMeshComponent;
class UAnimSequenceBase;
class UMGFImplementationAbilityAttributeSet;
class UDataTable;
class APawn;
class AController;

USTRUCT()
struct FMGFImplementationAttributeApplication
{
	GENERATED_BODY()

	// Ability set to grant
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMGFImplementationAbilityAttributeSet> AttributeSetType;

	// Data table reference to initialize the attributes with, if any (can be left unset)
	UPROPERTY(EditAnywhere)
	UDataTable* InitializationData = nullptr;
};

UCLASS(meta=(BlueprintSpawnableComponent))
class UMGFImplementationAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Ability)
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = Ability)
	TArray<FMGFImplementationAttributeApplication> DefaultAttributes;

	//~ Begin UAbilitySystemComponent interface
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	//~ End UAbilitySystemComponent interface

	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	UFUNCTION(BlueprintCallable, Category = "MGFImplementation|Abilities")
	FGameplayAbilitySpecHandle GrantAbilityOfType(TSubclassOf<UGameplayAbility> AbilityType, bool bRemoveAfterActivation);

protected:
	void GrantDefaultAbilitiesAndAttributes();

	UFUNCTION() // UFunction to be able to bind with dynamic delegate
	void OnPawnControllerChanged(APawn* Pawn, AController* NewController);
	TArray<FGameplayAbilitySpecHandle> DefaultAbilityHandles;

	UPROPERTY(transient)
	TArray<UAttributeSet*> AddedAttributes;
};
