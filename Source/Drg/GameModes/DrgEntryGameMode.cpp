// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgEntryGameMode.h"

#include "DrgGameStateManagerSubsystem.h"

void ADrgEntryGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UDrgGameStateManagerSubsystem* GameStateManager = GameInstance->GetSubsystem<UDrgGameStateManagerSubsystem>())
		{
			GameStateManager->StartLoadingSequence();
		}
	}
}
