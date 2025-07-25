// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

ADrgGameModeBase::ADrgGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/Characters/Player/BP_DrgPlayerCharacter"));
	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(
		TEXT("/Game/Characters/Player/BP_DrgPlayerController"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
