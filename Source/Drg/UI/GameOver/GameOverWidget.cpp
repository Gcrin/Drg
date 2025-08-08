#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

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

	// 테스트용 더미 데이터 설정
	FGameResultData DummyData;
	DummyData.MonstersKilled = 50;
	DummyData.GoldEarned = 1250;
	DummyData.SurvivalTimeSeconds = 185.5f; // 3분 5초
	DummyData.FinalLevel = 8;
	
	SetGameResult(DummyData, false); // 패배로 테스트
}

void UGameOverWidget::SetGameResult(const FGameResultData& ResultData, bool bIsVictory)
{
	GameResultData = ResultData;
	
	// 결과 텍스트 업데이트
	UpdateResultTexts();
	
	// 승리/패배에 따른 상단 텍스트 변경
	// 나중에 상단 타이틀 텍스트 위젯 추가 후 구현
	if (bIsVictory)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameOver: Victory! Boss defeated!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameOver: Defeat! Dragon has fallen..."));
	}
}

void UGameOverWidget::OnRestartClicked()
{
	// GameStateManager->ChangeState(InGame) 호출 예정
	UE_LOG(LogTemp, Warning, TEXT("GameOver: Restart Button Clicked"));
	
	// 임시: 위젯 숨기기
	SetVisibility(ESlateVisibility::Hidden);
	
	// 나중에 구현 예정:
	// if (UGameInstance* GI = GetGameInstance())
	// {
	//     if (UDrgGameStateManagerSubsystem* Manager = GI->GetSubsystem<UDrgGameStateManagerSubsystem>())
	//     {
	//         Manager->ChangeState(EGameFlowState::InGame);
	//     }
	// }
}

void UGameOverWidget::OnMainMenuClicked()
{
	// GameStateManager->ChangeState(MainMenu) 호출 예정
	UE_LOG(LogTemp, Warning, TEXT("GameOver: Main Menu Button Clicked"));
	
	// 임시: 위젯 숨기기
	SetVisibility(ESlateVisibility::Hidden);
	
	// 나중에 구현 예정:
	// if (UGameInstance* GI = GetGameInstance())
	// {
	//     if (UDrgGameStateManagerSubsystem* Manager = GI->GetSubsystem<UDrgGameStateManagerSubsystem>())
	//     {
	//         Manager->ChangeState(EGameFlowState::MainMenu);
	//     }
	// }
}

void UGameOverWidget::UpdateResultTexts()
{
	if (MonstersKilledText)
	{
		FString MonstersText = FString::Printf(TEXT("처치한 몬스터: %d마리"), GameResultData.MonstersKilled);
		MonstersKilledText->SetText(FText::FromString(MonstersText));
	}

	if (GoldEarnedText)
	{
		FString GoldText = FString::Printf(TEXT("획득한 골드: %d"), GameResultData.GoldEarned);
		GoldEarnedText->SetText(FText::FromString(GoldText));
	}

	if (SurvivalTimeText)
	{
		FString TimeText = FString::Printf(TEXT("생존 시간: %s"), *GameResultData.GetFormattedSurvivalTime());
		SurvivalTimeText->SetText(FText::FromString(TimeText));
	}

	if (FinalLevelText)
	{
		FString LevelText = FString::Printf(TEXT("최종 레벨: %d"), GameResultData.FinalLevel);
		FinalLevelText->SetText(FText::FromString(LevelText));
	}
}