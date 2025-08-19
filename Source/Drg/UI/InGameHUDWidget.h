#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Drg/GameModes/DrgMessageTypes.h"
#include "InGameHUDWidget.generated.h"

class UDrgBossWidget;
class UDrgDamageWidget;
class UCanvasPanel;
class UProgressBar;
class UTextBlock;
class UHorizontalBox;
class UDrgSkillWidget;
class UWidgetComponent;

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

	// 에디터에서 Text 수정
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnHealthChanged(float Health, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnStaminaChanged(float Stamina, float MaxStamina);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnExperienceChanged(float Exp, float MaxExp);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnCharacterLevelChanged(float Level);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnHealthRegenChanged(float HealthRegen);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnStaminaRegenChanged(float StaminaRegen);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnAttackDamageChanged(float AttackDamage);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnDefenseChanged(float Defense);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnAttackSpeedChanged(float AttackSpeed);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnMoveSpeedChanged(float MoveSpeed);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnPickupRadiusChanged(float PickupRadius);

	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnUpdateKillCount(int32 NewKillCount);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnTimerUpdated(int32 Minutes, int32 Seconds);
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD")
	void OnWaveNumberChanged(int32 NewWaveNumber, bool bIsLastWave);

protected:
	// === 경험치바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ExperienceBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ExperienceText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	// === 체력바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	// === 스태미나 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> StaminaText;

	// === 기타 능력치 ===
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

	// === 킬 카운트 ===
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCountText;

	// === 타이머 ===
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

	// === 웨이브 ===
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentWaveText;

	// 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "Drg|HUD")
	TSubclassOf<UDrgSkillWidget> SkillWidgetClass;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AbilityListBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> EffectListBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidget> AbilityTitle;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidget> EffectTitle;

	// 데미지
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> DamageFontCanvas;

	// 보스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|UI")
	TSubclassOf<UUserWidget> BossWidgetClass;
	FGameplayTag BossTag;
	
private:
	// 타이머 관련
	UFUNCTION()
	void HandleTimeUpdated(float SurvivalTimeSeconds);
	FDelegateHandle TimeUpdatedHandle;

	// 킬 카운트 관련
	UFUNCTION()
	void HandleKillCountChanged(int32 NewKillCount);
	FDelegateHandle KillCountChangedHandle;

	// 웨이브 관련
	UFUNCTION()
	void HandleWaveNumberChanged(int32 NewWaveNumber, bool bIsLastWave);
	FDelegateHandle WaveNumberChangedHandle;

	// 어빌리티, 이펙트 관련
	UFUNCTION()
	void HandleEquippedSkillsChanged();
	FDelegateHandle EquippedSkillsChangedHandle;

	// 프로그레스 바 업데이트
	void UpdateHealthBar(float Health, float MaxHealth);
	void UpdateStaminaBar(float Stamina, float MaxStamina);
	void UpdateExperienceBar(float Exp, float MaxExp);

	// 메시지 시스템 관련
	void OnAttributeChangedReceived(FGameplayTag Channel, const FDrgAttributeChangeMessage& Message);
	FGameplayMessageListenerHandle AttributeChangeMessageListenerHandle;

	void OnDamagedActor(FGameplayTag Channel, const FDrgDamageMessage& Message);
	FGameplayMessageListenerHandle DamagedActorMessageListenerHandle;

	float CurrentHealth = 0.0f;
	float CurrentMaxHealth = 0.0f;

	float CurrentStamina = 0.0f;
	float CurrentMaxStamina = 0.0f;

	float CurrentExperience = 0.0f;
	float CurrentMaxExperience = 0.0f;
	float CurrentCharacterLevel = 0.f;

	float CurrentHealthRegen = 0.0f;
	float CurrentStaminaRegen = 0.0f;
	float CurrentAttackDamage = 0.0f;
	float CurrentDefense = 0.0f;
	float CurrentAttackSpeed = 0.0f;
	float CurrentMoveSpeed = 0.0f;
	float CurrentPickupRadius = 0.0f;
};
