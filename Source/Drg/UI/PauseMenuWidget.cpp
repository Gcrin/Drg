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
	
	if (UDrgGameStateManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UDrgGameStateManagerSubsystem>())
	{
		Manager->ResumeGame();
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