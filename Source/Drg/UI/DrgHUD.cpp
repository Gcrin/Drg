#include "DrgHUD.h"
#include "Blueprint/UserWidget.h"
#include "Drg/UI/GameOver/GameOverWidget.h"
#include "Drg/UI/GameOver/GameResultData.h"

void ADrgHUD::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 시작 시 인게임 HUD 표시
	ShowInGameHUD();
}

void ADrgHUD::ShowGameOverUI(bool bIsVictory)
{
	if (!GameOverWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameOverWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 기존 게임오버 위젯이 있으면 제거
	HideGameOverUI();

	// 새 게임오버 위젯 생성
	CurrentGameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetClass);
	if (CurrentGameOverWidget)
	{
		// 더미 데이터로 일단 표시 (나중에 실제 데이터로 교체)
		FGameResultData DummyData;
		DummyData.MonstersKilled = 42;
		DummyData.GoldEarned = 1250;
		DummyData.SurvivalTimeSeconds = 185.5f;
		DummyData.FinalLevel = 7;

		CurrentGameOverWidget->SetGameResult(DummyData, bIsVictory);
		CurrentGameOverWidget->AddToViewport();
		
		// UI 전용 입력 모드로 전환
		SetUIInputMode(true);
		
		UE_LOG(LogTemp, Warning, TEXT("게임오버 UI 표시됨! Victory: %s"), bIsVictory ? TEXT("true") : TEXT("false"));
	}
}

void ADrgHUD::HideGameOverUI()
{
	if (CurrentGameOverWidget)
	{
		CurrentGameOverWidget->RemoveFromParent();
		CurrentGameOverWidget = nullptr;
		
		// 게임 입력 모드로 복원
		SetUIInputMode(false);
	}
}

void ADrgHUD::ShowPauseMenu()
{
	if (!PauseMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PauseMenuWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 이미 있으면 제거
	HidePauseMenu();

	// 일시정지 메뉴 생성
	CurrentPauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	if (CurrentPauseMenuWidget)
	{
		CurrentPauseMenuWidget->AddToViewport();
		SetUIInputMode(true);
		
		// 게임 일시정지
		GetWorld()->GetFirstPlayerController()->SetPause(true);
		
		UE_LOG(LogTemp, Warning, TEXT("일시정지 메뉴 표시됨"));
	}
}

void ADrgHUD::HidePauseMenu()
{
	if (CurrentPauseMenuWidget)
	{
		CurrentPauseMenuWidget->RemoveFromParent();
		CurrentPauseMenuWidget = nullptr;
		
		// 게임 재개
		GetWorld()->GetFirstPlayerController()->SetPause(false);
		SetUIInputMode(false);
	}
}

void ADrgHUD::ShowInGameHUD()
{
	if (InGameHUDWidgetClass && !CurrentInGameHUDWidget)
	{
		CurrentInGameHUDWidget = CreateWidget<UUserWidget>(GetWorld(), InGameHUDWidgetClass);
		if (CurrentInGameHUDWidget)
		{
			CurrentInGameHUDWidget->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("인게임 HUD 표시됨"));
		}
	}
}

void ADrgHUD::HideInGameHUD()
{
	if (CurrentInGameHUDWidget)
	{
		CurrentInGameHUDWidget->RemoveFromParent();
		CurrentInGameHUDWidget = nullptr;
	}
}

void ADrgHUD::SetUIInputMode(bool bUIOnly)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		if (bUIOnly)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
		else
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}