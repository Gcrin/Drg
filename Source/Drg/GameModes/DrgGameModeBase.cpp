// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/Player/DrgPlayerController.h"

ADrgGameModeBase::ADrgGameModeBase()
{
	DefaultPawnClass = ADrgBaseCharacter::StaticClass();
}
