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
	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void SetGameResult(const FGameResultData& ResultData, bool bIsVictory = false);

protected:
	// 버튼 이벤트 함수들
	UFUNCTION()
	void OnRestartClicked();
	
	UFUNCTION()
	void OnMainMenuClicked();

	// 게임 결과 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Game Result")
	FGameResultData GameResultData;

	// UMG 위젯과 바인딩될 UI 요소들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	// 결과 표시 텍스트들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameResultTitleText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MonstersKilledText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldEarnedText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SurvivalTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FinalLevelText;

private:
	// UI 텍스트를 업데이트하는 함수
	void UpdateResultTexts();
};
