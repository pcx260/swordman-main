// Copyright Epic Games, Inc. All Rights Reserved.

#include "locomotionsystemGameMode.h"
#include "locomotionsystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AlocomotionsystemGameMode::AlocomotionsystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
