#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultData.h"
#include "GameOverWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 게임 오버 UI 위젯
 * 게임 결과를 표시하고 재시작/메인메뉴 기능을 제공
 */
UCLASS()
class DRG_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 게임 결과 데이터를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Drg|UI")
	void SetGameResult(const FGameResultData& ResultData, bool bIsVictory = false);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|UI")
	void OnResultReceived(bool bIsVictory, const FGameResultData& ResultData);

protected:
	// 버튼 이벤트 함수들
	UFUNCTION()
	void OnRestartClicked();
	UFUNCTION()
	void OnMainMenuClicked();

	// UMG 위젯과 바인딩될 UI 요소들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	// 결과 표시 텍스트들
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> GameResultTitleText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> FinalWaveText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SurvivalTimeText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCountText;
	// 스탯 표시 텍스트들
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> StaminaText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ExperienceText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterLevelText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthRegenText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> StaminaRegenText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackDamageText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DefenseText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackSpeedText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MoveSpeedText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PickupRadiusText;
};
