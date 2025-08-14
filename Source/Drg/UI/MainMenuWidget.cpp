#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Drg/GameModes/DrgGameStateManagerSubsystem.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
	}
	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitGameClicked);
	}
}

void UMainMenuWidget::OnStartGameClicked()
{
	if (UDrgGameStateManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UDrgGameStateManagerSubsystem>())
	{
		Manager->ChangeState(EGameFlowState::InGame);
	}
}

void UMainMenuWidget::OnQuitGameClicked()
{
	if (UDrgGameStateManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UDrgGameStateManagerSubsystem>())
	{
		Manager->ChangeState(EGameFlowState::Quitting);
	}
}