// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"


void UDrgGameStateManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ChangeState(EGameFlowState::MainMenu);
}

void UDrgGameStateManagerSubsystem::ChangeState(EGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		UE_LOG(LogTemp, Display, TEXT("동일한 스테이트로는 변경할 수 없습니다."));
		return;
	}

	CurrentState = NewState;

	switch (CurrentState)
	{
		case EGameFlowState::MainMenu:
			HandleMainMenuState();
			break;
		case EGameFlowState::InGame:
			HandleInGameState();
			break;
		case EGameFlowState::PostGame:
			HandleQuittingState();
			break;
		// 추가적인 상태
	}
}

void UDrgGameStateManagerSubsystem::HandleMainMenuState()
{
	UE_LOG(LogTemp, Display, TEXT("메인 메뉴로 이동"));
	UGameplayStatics::OpenLevel(this, FName("L_MainMenu"));
}

void UDrgGameStateManagerSubsystem::HandleInGameState()
{
	UE_LOG(LogTemp, Display, TEXT("게임 레벨로 이동"));
	UGameplayStatics::OpenLevel(this, FName("L_InGame"));
}

void UDrgGameStateManagerSubsystem::HandleQuittingState()
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}
