#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Drg/GameModes/DrgMessageTypes.h"
#include "DrgHUD.generated.h"

class UGameOverWidget;
class UUserWidget;

/**
 * 모든 UI를 관리하는 HUD 클래스
 */
UCLASS()
class DRG_API ADrgHUD : public AHUD
{
	GENERATED_BODY()

public:

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnGameStateChanged(FGameplayTag Channel, const FDrgGameStateChangeMessage& Message);
	
	// === 위젯 클래스들 ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|UI")
	TSubclassOf<UUserWidget> InGameHUDWidgetClass;

private:
	void ShowGameOverUI(bool bIsVictory = false);
	void ShowPauseMenu();
	void ShowInGameHUD();

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentWidget;

	FGameplayMessageListenerHandle GameStateListenerHandle;
};