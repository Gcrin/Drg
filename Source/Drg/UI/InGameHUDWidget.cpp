#include "InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	AttributeChangeMessageListenerHandle = MessageSubsystem.RegisterListener(
	   DrgGameplayTags::Event_Broadcast_AttributeChanged,
	   this,
	   &UInGameHUDWidget::OnAttributeChangedReceived
	);
	
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		ADrgPlayerCharacter* PlayerCharacter = Cast<ADrgPlayerCharacter>(PlayerController->GetPawn());
		if (PlayerCharacter)
		{
			const UDrgAttributeSet* Attributes = PlayerCharacter->GetAttributeSet();
			if (Attributes)
			{
				// 현재 값들을 AttributeSet에서 직접 가져와 캐시 변수에 저장
				CurrentHealth = Attributes->GetHealth();
				CurrentHealth = Attributes->GetMaxHealth();
				CurrentExperience = Attributes->GetExperience();
				CurrentMaxExperience = Attributes->GetMaxExperience();
				CurrentLevel = Attributes->GetCharacterLevel();
				CurrentAttackDamage = Attributes->GetAttackDamage();

				// 저장된 값으로 UI 업데이트 함수를 즉시 호출
				OnHealthChanged(CurrentHealth, CurrentMaxHealth);
				OnExperienceChanged(CurrentExperience, CurrentMaxExperience, CurrentLevel);
				OnAttackDamageChanged(CurrentAttackDamage);
			}
		}
	}
	
	GameStartTime = GetWorld()->GetTimeSeconds();
	GetWorld()->GetTimerManager().SetTimer(
		TimerDisplayHandle,
		this,
		&UInGameHUDWidget::UpdateTimerDisplay,
		1.0f,
		true
		);
}

void UInGameHUDWidget::OnAttributeChangedReceived(FGameplayTag Channel, const FDrgAttributeChangeMessage& Message)
{
	if (Message.AttributeType == EAttributeType::Health)
	{
		CurrentHealth = Message.NewValue;
		OnHealthChanged(CurrentHealth, CurrentMaxHealth);
	}
	else if (Message.AttributeType == EAttributeType::MaxHealth)
	{
		CurrentMaxHealth = Message.NewValue;
		OnHealthChanged(CurrentHealth, CurrentMaxHealth);
	}
	else if (Message.AttributeType == EAttributeType::Experience)
	{
		CurrentExperience = Message.NewValue;
		OnExperienceChanged(CurrentExperience, CurrentMaxExperience, CurrentLevel);
	}
	else if (Message.AttributeType == EAttributeType::MaxExperience)
	{
		CurrentMaxExperience = Message.NewValue;
		OnExperienceChanged(CurrentExperience, CurrentMaxExperience, CurrentLevel);
	}
	else if (Message.AttributeType == EAttributeType::Level)
	{
		CurrentLevel = Message.NewValue;
		OnExperienceChanged(CurrentExperience, CurrentMaxExperience, CurrentLevel);
	}
	else if (Message.AttributeType == EAttributeType::AttackDamage)
	{
		CurrentAttackDamage = Message.NewValue;
		OnAttackDamageChanged(CurrentAttackDamage);
	}
}

void UInGameHUDWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerDisplayHandle);
	if (AttributeChangeMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(AttributeChangeMessageListenerHandle);
	}
	Super::NativeDestruct();
}

void UInGameHUDWidget::OnHealthChanged(float Health, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		float HealthPercent = Health / MaxHealth;
		HealthBar->SetPercent(HealthPercent);
	}
	
	if (HealthText)
	{
		FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UInGameHUDWidget::OnExperienceChanged(float Exp, float MaxExp, float Level)
{
	if (ExperienceBar && MaxExp > 0.0f)
	{
		float ExpPercent = Exp / MaxExp;
		ExperienceBar->SetPercent(ExpPercent);
	}

	if (ExperienceText)
	{
		FString ExperienceString = FString::Printf(TEXT("%.0f/%.0f"), Exp, MaxExp);
		ExperienceText->SetText(FText::FromString(ExperienceString));
	}
	
	if (LevelText)
	{
		FString LevelString = FString::Printf(TEXT("Lv %.0f"), Level);
		LevelText->SetText(FText::FromString(LevelString));
	}
}

void UInGameHUDWidget::OnAttackDamageChanged(float AttackDamage)
{
	if (AttackDamageText)
	{
		FString AttackDamageString = FString::Printf(TEXT("공격력: %.0f"), AttackDamage);
		AttackDamageText->SetText(FText::FromString(AttackDamageString));
	}
}

void UInGameHUDWidget::UpdateTimerDisplay()
{
	if (TimerText)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds() - GameStartTime;
		const int32 TotalSeconds = FMath::FloorToInt(CurrentTime);
		const int32 Minutes = TotalSeconds / 60;
		const int32 Seconds = TotalSeconds % 60;
	
		const FString TimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        
		TimerText->SetText(FText::FromString(TimerString));
	}
}