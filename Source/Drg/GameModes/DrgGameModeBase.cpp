// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "DrgGameStateBase.h"


ADrgGameModeBase::ADrgGameModeBase() {}

void ADrgGameModeBase::SetCurrentMatchState(EMatchState NewState)
{
	ADrgGameStateBase* DrgGameState = Cast<ADrgGameStateBase>(GameState);

	if (ensureMsgf(DrgGameState, TEXT("DrgGameMode/MatchState/Failed/콜스택 확인")))
	{
		DrgGameState->SetMatchState(NewState);
	}
}
