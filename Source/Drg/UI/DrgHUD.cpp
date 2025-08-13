#include "DrgHUD.h"
#include "Blueprint/UserWidget.h"
#include "Drg/UI/GameOver/GameOverWidget.h"
#include "Drg/UI/GameOver/GameResultData.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Kismet/GameplayStatics.h"

void ADrgHUD::BeginPlay()
{
	Super::BeginPlay();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	GameStateListenerHandle = MessageSubsystem.RegisterListener(
		DrgGameplayTags::Event_Broadcast_StateChanged,
		this,
		&ADrgHUD::OnGameStateChanged
	);
	
	// 게임 시작 시 인게임 HUD 표시
	ShowInGameHUD();
}

void ADrgHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(GameStateListenerHandle);
	Super::EndPlay(EndPlayReason);
}

void ADrgHUD::OnGameStateChanged(FGameplayTag Channel, const FDrgGameStateChangeMessage& Message)
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	switch (Message.NewState)
	{
	case EGameFlowState::InGame:
		ShowInGameHUD();
		break;
	case EGameFlowState::Pause:
		ShowPauseMenu();
		break;
	case EGameFlowState::PostGame:
		const bool bIsVictory = (Message.GameResult == EGameResult::Victory);
		ShowGameOverUI(bIsVictory);
		break;
	default:
		// MainMenu 등 다른 상태에서는 아무것도 띄우지 않음
		break;
	}
}

void ADrgHUD::ShowGameOverUI(bool bIsVictory)
{
	if (!GameOverWidgetClass) return;

	UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(GetOwningPlayerController(), GameOverWidgetClass);
	if (GameOverWidget)
	{
		FGameResultData ResultData;
		GameOverWidget->SetGameResult(ResultData, bIsVictory);
        
		GameOverWidget->AddToViewport();
		CurrentWidget = GameOverWidget;
	}
}

void ADrgHUD::ShowPauseMenu()
{
	if (!PauseMenuWidgetClass) return;
    
	CurrentWidget = CreateWidget(GetOwningPlayerController(), PauseMenuWidgetClass);
	if (CurrentWidget) CurrentWidget->AddToViewport();
}

void ADrgHUD::ShowInGameHUD()
{
	if (!InGameHUDWidgetClass) return;

	CurrentWidget = CreateWidget(GetOwningPlayerController(), InGameHUDWidgetClass);
	if (CurrentWidget) CurrentWidget->AddToViewport();
}
