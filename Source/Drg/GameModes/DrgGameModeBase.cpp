// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "DrgGameStateBase.h"

ADrgGameModeBase::ADrgGameModeBase()
{
	// 하드 코딩 제거 -> BP 리플렉션 등록으로 교체
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/Characters/Player/BP_DrgPlayerCharacter"));
	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(
		TEXT("/Game/Characters/Player/BP_DrgPlayerController"));
	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(
		TEXT("/Game/Core/GameModes/BP_DrgGameState"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	if (GameStateBPClass.Class != NULL)
	{
		GameStateClass = GameStateBPClass.Class;
	}
}

void ADrgGameModeBase::SetCurrentMatchState(EMatchState NewState)
{
	ADrgGameStateBase* DrgGameState = Cast<ADrgGameStateBase>(GameState);

	if (ensureMsgf(DrgGameState, TEXT("DrgGameMode/MatchState/Failed/콜스택 확인")))
	{
		DrgGameState->SetMatchState(NewState);
	}
}