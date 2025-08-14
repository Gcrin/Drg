#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

/**
 * 일시정지 메뉴 UI 위젯
 */
UCLASS()
class DRG_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	// 버튼 이벤트 함수들
	UFUNCTION()
	void OnResumeClicked();
	
	UFUNCTION()
	void OnMainMenuClicked();

	// UMG 위젯과 바인딩될 버튼들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;
};