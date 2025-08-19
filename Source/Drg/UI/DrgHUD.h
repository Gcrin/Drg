#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Drg/GameModes/DrgMessageTypes.h"
#include "DrgHUD.generated.h"

class UDrgDamageWidget;
class UGameOverWidget;
class UUserWidget;
class UWidgetComponent;

/**
 * 모든 UI를 관리하는 HUD 클래스
 */
UCLASS()
class DRG_API ADrgHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADrgHUD();

	void RequestDamageNumber(float Damage, FVector WorldLocation);
	void ReturnToPool(UWidgetComponent* ComponentToReturn);

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

	// === 위젯 컴포넌트 풀링 ===
	UPROPERTY(EditDefaultsOnly, Category = "Drg|UI|DamageFont")
	TSubclassOf<UDrgDamageWidget> DamageWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Drg|UI|DamageFont")
	int32 DamagePoolSize = 500;

private:
	void ShowGameOverUI(bool bIsVictory = false);
	void ShowPauseMenu();
	void ShowInGameHUD();

	void InitializeDamagePool();
	UPROPERTY()
	TArray<TObjectPtr<UWidgetComponent>> InactiveDamagePool;
	UPROPERTY()
	TArray<TObjectPtr<UWidgetComponent>> ActiveDamagePool;

	UPROPERTY()
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> InGameHUDWidget;

	FGameplayMessageListenerHandle GameStateListenerHandle;
};
