#include "InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 게임 시작 시간 기록
	GameStartTime = GetWorld()->GetTimeSeconds();
	
	// AttributeSet 이벤트 구독
	// AttributeSet->OnHealthChanged.AddUObject(this, &UInGameHUDWidget::OnHealthChanged);
	// AttributeSet->OnExperienceChanged.AddUObject(this, &UInGameHUDWidget::OnExperienceChanged);
	// UpgradeComponent->OnSkillSlotsChanged.AddUObject(this, &UInGameHUDWidget::OnSkillSlotsChanged);
	
	// 초기 더미 데이터 설정 (테스트용)
	OnHealthChanged(100.0f, 100.0f);
	OnExperienceChanged(25.0f, 100.0f, 1.0f);
	OnStatsChanged(0, 0);
	
	UE_LOG(LogTemp, Warning, TEXT("InGame HUD 초기화됨!"));
}

void UInGameHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// 매 프레임마다 타이머 업데이트
	if (TimerText)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float ElapsedTime = CurrentTime - GameStartTime;
		
		int32 Minutes = FMath::FloorToInt(ElapsedTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(ElapsedTime) % 60;
		FString TimeString = FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
		TimerText->SetText(FText::FromString(TimeString));
	}
}

void UInGameHUDWidget::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		float HealthPercent = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(HealthPercent);
	}
	
	if (HealthText)
	{
		FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UInGameHUDWidget::OnExperienceChanged(float CurrentExp, float MaxExp, float Level)
{
	if (ExperienceBar && MaxExp > 0.0f)
	{
		float ExpPercent = CurrentExp / MaxExp;
		ExperienceBar->SetPercent(ExpPercent);
	}
	
	if (LevelText)
	{
		FString LevelString = FString::Printf(TEXT("Lv %.0f"), Level);
		LevelText->SetText(FText::FromString(LevelString));
	}
}

void UInGameHUDWidget::OnStatsChanged(int32 MonstersKilled, int32 Gold)
{
	if (MonstersKilledText)
	{
		FString MonsterString = FString::Printf(TEXT("%d"), MonstersKilled);
		MonstersKilledText->SetText(FText::FromString(MonsterString));
	}
	
	if (GoldText)
	{
		FString GoldString = FString::Printf(TEXT("%d"), Gold);
		GoldText->SetText(FText::FromString(GoldString));
	}
}

void UInGameHUDWidget::OnSkillSlotsChanged(const TArray<UDrgAbilityDataAsset*>& OwnedSkills)
{
	// 스킬 슬롯 1~3에 아이콘 설정
	TArray<UImage*> SkillSlots = {SkillSlot1, SkillSlot2, SkillSlot3};
	
	for (int32 i = 0; i < 3; ++i)
	{
		if (SkillSlots[i])
		{
			if (i < OwnedSkills.Num() && OwnedSkills[i] && OwnedSkills[i]->AbilityLevelData.Num() > 0)
			{
				// 스킬 아이콘 설정
				UTexture2D* SkillIcon = OwnedSkills[i]->AbilityLevelData[0].AbilityIcon.LoadSynchronous();
				if (SkillIcon)
				{
					SkillSlots[i]->SetBrushFromTexture(SkillIcon);
					SkillSlots[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else
			{
				// 빈 슬롯 처리
				SkillSlots[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("스킬 슬롯 업데이트 완료: %d개 스킬"), OwnedSkills.Num());
}