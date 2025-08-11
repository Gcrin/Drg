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
	if (GameResultTitleText)
	{
		if (bIsVictory)
		{
			GameResultTitleText->SetText(FText::FromString(TEXT("축하합니다!\n보스를 물리쳤습니다!")));
		}
		else
		{
			GameResultTitleText->SetText(FText::FromString(TEXT("게임 오버\n용감한 드래곤이 쓰러졌습니다...")));
		}
	}

	// 결과 텍스트 업데이트
	UpdateResultTexts();
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