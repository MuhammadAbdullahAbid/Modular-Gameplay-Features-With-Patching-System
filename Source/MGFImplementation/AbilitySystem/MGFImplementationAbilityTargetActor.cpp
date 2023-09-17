// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGFImplementationAbilityTargetActor.h"
#include "MGFImplementation/MGFImplementation.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/BlueprintGeneratedClass.h"

void AMGFImplementationAbilityTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	
	RecieveStartTargeting(Ability);
}

bool AMGFImplementationAbilityTargetActor::IsConfirmTargetingAllowed()
{
	return K2_IsConfirmTargetingAllowed();
}

void AMGFImplementationAbilityTargetActor::ConfirmTargetingAndContinue()
{
	if (SourceActor && ensureAlways(ShouldProduceTargetData()))
	{
		auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
		{
			return Func && ensure(Func->GetOuter())
				&& Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
		};
		/*UE_LOG(
			!ImplementedInBlueprint(GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(AMGFImplementationAbilityTargetActor, MakeTargetData))),
			LogTemp,
			Error,
			TEXT("`%s` does not implement `MakeTargetData`. Invalid target data will be returned."),
			*GetClass()->GetName()
		);*/

		UE_LOG(LogTemp, Error, TEXT("`%s` does not implement `MakeTargetData`. Invalid target data will be returned."),
			*GetClass()->GetName());

		FGameplayAbilityTargetDataHandle Handle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

bool AMGFImplementationAbilityTargetActor::K2_IsConfirmTargetingAllowed_Implementation()
{
	return Super::IsConfirmTargetingAllowed();
}