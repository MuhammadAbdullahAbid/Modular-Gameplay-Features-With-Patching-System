// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGFImplementationGameMode.h"
#include "MGFImplementationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMGFImplementationGameMode::AMGFImplementationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
