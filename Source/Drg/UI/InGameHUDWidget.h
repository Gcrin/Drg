#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// AttributeSet 이벤트 구독용 함수들
	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnExperienceChanged(float CurrentExp, float MaxExp, float Level);

	UFUNCTION()
	void OnStatsChanged(int32 MonstersKilled, int32 Gold);

	UFUNCTION()
	void OnSkillSlotsChanged(const TArray<UDrgAbilityDataAsset*>& OwnedSkills);

protected:
	// === 경험치바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ExperienceBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	// === 체력바 관련 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	// === 타이머 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

	// === 스탯 표시 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MonstersKilledText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	// === 스킬 슬롯들 ===
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillSlot1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillSlot2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillSlot3;

private:
	// 타이머 관련
	float GameStartTime = 0.0f;
};
