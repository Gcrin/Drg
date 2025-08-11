#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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
	virtual void BeginPlay() override;

	// === 게임오버 UI ===
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOverUI(bool bIsVictory = false);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideGameOverUI();

	// === 일시정지 UI ===
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePauseMenu();

	// === 인게임 HUD ===
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInGameHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInGameHUD();

protected:
	// === 위젯 클래스들 ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Classes")
	TSubclassOf<UUserWidget> InGameHUDWidgetClass;

private:
	// === 현재 활성화된 위젯들 ===
	UPROPERTY()
	TObjectPtr<UGameOverWidget> CurrentGameOverWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentPauseMenuWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentInGameHUDWidget;

	// === 내부 함수들 ===
	void SetUIInputMode(bool bUIOnly);
};