#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class DRG_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnStartGameClicked();
	
	UFUNCTION()
	void OnQuitGameClicked();
	
	// UMG 위젯과 바인딩될 버튼들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;
};
