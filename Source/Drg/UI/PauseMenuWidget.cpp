#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Drg/GameModes/DrgGameStateManagerSubsystem.h"
#include "Drg/UI/DrgHUD.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트 바인딩
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("PauseMenu: Resume Button Clicked"));
	
	// HUD를 통해 일시정지 메뉴 숨기기
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ADrgHUD* HUD = PC->GetHUD<ADrgHUD>())
			{
				HUD->HidePauseMenu();
			}
		}
	}
}

void UPauseMenuWidget::OnMainMenuClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("PauseMenu: Main Menu Button Clicked"));
	
	// GameStateManager를 통해 메인메뉴로 이동
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDrgGameStateManagerSubsystem* Manager = GI->GetSubsystem<UDrgGameStateManagerSubsystem>())
		{
			Manager->ChangeState(EGameFlowState::MainMenu);
		}
	}
}