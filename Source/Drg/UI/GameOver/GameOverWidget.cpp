#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Drg/GameModes/DrgGameStateManagerSubsystem.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트 바인딩
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnMainMenuClicked);
	}
}

void UGameOverWidget::SetGameResult(const FGameResultData& ResultData, bool bIsVictory)
{	
	OnResultReceived(bIsVictory, ResultData);
}

void UGameOverWidget::OnRestartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("GameOver: Restart Button Clicked"));

	// GameStateManager를 통해 게임 재시작
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDrgGameStateManagerSubsystem* Manager = GI->GetSubsystem<UDrgGameStateManagerSubsystem>())
		{
			Manager->ChangeState(EGameFlowState::InGame);
		}
	}
}

void UGameOverWidget::OnMainMenuClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("GameOver: Main Menu Button Clicked"));
	
	// GameStateManager를 통해 메인메뉴로 이동
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDrgGameStateManagerSubsystem* Manager = GI->GetSubsystem<UDrgGameStateManagerSubsystem>())
		{
			Manager->ChangeState(EGameFlowState::MainMenu);
		}
	}
}