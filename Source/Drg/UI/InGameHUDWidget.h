#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Drg/GameModes/DrgMessageTypes.h"
#include "InGameHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * 인게임 HUD UI 위젯
 */
UCLASS()
class DRG_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// AttributeSet 이벤트 구독용 함수들
	UFUNCTION()
	void OnHealthChanged(float Health, float MaxHealth);

	UFUNCTION()
	void OnExperienceChanged(float Exp, float MaxExp, float Level);

	UFUNCTION()
	void OnAttackDamageChanged(float AttackDamage);
protected:
	// === 경험치바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ExperienceBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ExperienceText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	// === 체력바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	// 공격력
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackDamageText;

	// === 타이머 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

private:
	// 타이머 관련
	void UpdateTimerDisplay();
	FTimerHandle TimerDisplayHandle;
	float GameStartTime = 0.0f;

	// 메시지 시스템 관련
	void OnAttributeChangedReceived(FGameplayTag Channel, const FDrgAttributeChangeMessage& Message);
	FGameplayMessageListenerHandle AttributeChangeMessageListenerHandle;
	float CurrentHealth = 0.0f;
	float CurrentMaxHealth = 0.0f;
	float CurrentExperience = 0.0f;
	float CurrentMaxExperience = 0.0f;
	float CurrentLevel = 0.f;
	float CurrentAttackDamage = 0.0f;
};
