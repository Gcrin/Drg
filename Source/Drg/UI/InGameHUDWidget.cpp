#include "InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "Drg/GameModes/DrgPlayerState.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	AttributeChangeMessageListenerHandle = MessageSubsystem.RegisterListener(
	   DrgGameplayTags::Event_Broadcast_AttributeChanged,
	   this,
	   &UInGameHUDWidget::OnAttributeChangedReceived
	);

	if (ADrgPlayerState* DrgPlayerState = GetOwningPlayerState<ADrgPlayerState>())
	{
		HandleKillCountChanged(DrgPlayerState->GetKillCount());
		KillCountChangedHandle = DrgPlayerState->OnKillCountChanged.AddUObject(this, &UInGameHUDWidget::HandleKillCountChanged);
	}
	
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
				CurrentMaxHealth = Attributes->GetMaxHealth();
				CurrentStamina = Attributes->GetStamina();
				CurrentMaxStamina = Attributes->GetMaxStamina();
				CurrentExperience = Attributes->GetExperience();
				CurrentMaxExperience = Attributes->GetMaxExperience();
				CurrentCharacterLevel = Attributes->GetCharacterLevel();
				CurrentHealthRegen = Attributes->GetHealthRegen();
				CurrentStaminaRegen = Attributes->GetStaminaRegen();
				CurrentAttackDamage = Attributes->GetAttackDamage();
				CurrentDefense = Attributes->GetDefense();
				CurrentAttackSpeed = Attributes->GetAttackSpeed();
				CurrentMoveSpeed = Attributes->GetMoveSpeed();
				CurrentPickupRadius = Attributes->GetPickupRadius();

				// 저장된 값으로 UI 업데이트 함수를 즉시 호출
				OnHealthChanged(CurrentHealth, CurrentMaxHealth);
				OnStaminaChanged(CurrentStamina, CurrentMaxStamina);
				OnExperienceChanged(CurrentExperience, CurrentMaxExperience);
				OnCharacterLevelChanged(CurrentCharacterLevel);
				OnHealthRegenChanged(CurrentHealthRegen);
				OnStaminaRegenChanged(CurrentStaminaRegen);
				OnAttackDamageChanged(CurrentAttackDamage);
				OnDefenseChanged(CurrentDefense);
				OnAttackSpeedChanged(CurrentAttackSpeed);
				OnMoveSpeedChanged(CurrentMoveSpeed);
				OnPickupRadiusChanged(CurrentPickupRadius);

				UpdateHealthBar(CurrentHealth, CurrentMaxHealth);
				UpdateStaminaBar(CurrentStamina, CurrentMaxStamina);
				UpdateExperienceBar(CurrentExperience, CurrentMaxExperience);
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

void UInGameHUDWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerDisplayHandle);
	if (AttributeChangeMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(AttributeChangeMessageListenerHandle);
	}
	if (ADrgPlayerState* DrgPlayerState = GetOwningPlayerState<ADrgPlayerState>())
	{
		DrgPlayerState->OnKillCountChanged.Remove(KillCountChangedHandle);
	}
	
	Super::NativeDestruct();
}

void UInGameHUDWidget::OnAttributeChangedReceived(FGameplayTag Channel, const FDrgAttributeChangeMessage& Message)
{
	if (Message.AttributeType == EAttributeType::Health)
	{
		CurrentHealth = Message.NewValue;
		OnHealthChanged(CurrentHealth, CurrentMaxHealth);
		UpdateHealthBar(CurrentHealth, CurrentMaxHealth);
	}
	else if (Message.AttributeType == EAttributeType::MaxHealth)
	{
		CurrentMaxHealth = Message.NewValue;
		OnHealthChanged(CurrentHealth, CurrentMaxHealth);
		UpdateHealthBar(CurrentHealth, CurrentMaxHealth);
	}
	else if (Message.AttributeType == EAttributeType::Stamina)
	{
		CurrentStamina = Message.NewValue;
		OnStaminaChanged(CurrentStamina, CurrentMaxStamina);
		UpdateStaminaBar(CurrentStamina, CurrentMaxStamina);
	}
	else if (Message.AttributeType == EAttributeType::MaxStamina)
	{
		CurrentMaxStamina = Message.NewValue;
		OnStaminaChanged(CurrentStamina, CurrentMaxStamina);
		UpdateStaminaBar(CurrentStamina, CurrentMaxStamina);
	}
	else if (Message.AttributeType == EAttributeType::Experience)
	{
		CurrentExperience = Message.NewValue;
		OnExperienceChanged(CurrentExperience, CurrentMaxExperience);
		UpdateExperienceBar(CurrentExperience, CurrentMaxExperience);
	}
	else if (Message.AttributeType == EAttributeType::MaxExperience)
	{
		CurrentMaxExperience = Message.NewValue;
		OnExperienceChanged(CurrentExperience, CurrentMaxExperience);
		UpdateExperienceBar(CurrentExperience, CurrentMaxExperience);
	}
	else if (Message.AttributeType == EAttributeType::CharacterLevel)
	{
		CurrentCharacterLevel = Message.NewValue;
		OnCharacterLevelChanged(CurrentCharacterLevel);
	}
	else if (Message.AttributeType == EAttributeType::HealthRegen)
	{
		CurrentHealthRegen = Message.NewValue;
		OnHealthRegenChanged(CurrentHealthRegen);
	}
	else if (Message.AttributeType == EAttributeType::StaminaRegen)
	{
		CurrentStaminaRegen = Message.NewValue;
		OnStaminaRegenChanged(CurrentStaminaRegen);
	}
	else if (Message.AttributeType == EAttributeType::AttackDamage)
	{
		CurrentAttackDamage = Message.NewValue;
		OnAttackDamageChanged(CurrentAttackDamage);
	}
	else if (Message.AttributeType == EAttributeType::Defense)
	{
		CurrentDefense = Message.NewValue;
		OnDefenseChanged(CurrentDefense);
	}
	else if (Message.AttributeType == EAttributeType::AttackSpeed)
	{
		CurrentAttackSpeed = Message.NewValue;
		OnAttackSpeedChanged(CurrentAttackSpeed);
	}
	else if (Message.AttributeType == EAttributeType::MoveSpeed)
	{
		CurrentMoveSpeed = Message.NewValue;
		OnMoveSpeedChanged(CurrentMoveSpeed);
	}
	else if (Message.AttributeType == EAttributeType::PickupRadius)
	{
		CurrentPickupRadius = Message.NewValue;
		OnPickupRadiusChanged(CurrentPickupRadius);
	}
}

void UInGameHUDWidget::HandleKillCountChanged(int32 NewKillCount)
{
	OnUpdateKillCount(NewKillCount);
}

void UInGameHUDWidget::UpdateHealthBar(float Health, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f) HealthBar->SetPercent(Health / MaxHealth);
}

void UInGameHUDWidget::UpdateStaminaBar(float Stamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0.0f)	StaminaBar->SetPercent(Stamina / MaxStamina);
}

void UInGameHUDWidget::UpdateExperienceBar(float Exp, float MaxExp)
{
	if (ExperienceBar && MaxExp > 0.0f)	ExperienceBar->SetPercent(Exp / MaxExp);
}

void UInGameHUDWidget::UpdateTimerDisplay()
{
	if (TimerText)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds() - GameStartTime;
		SurvivalTimeSeconds = FMath::FloorToInt(CurrentTime);
		const int32 Minutes = SurvivalTimeSeconds / 60;
		const int32 Seconds = SurvivalTimeSeconds % 60;
	
		OnTimerUpdated(Minutes, Seconds);
	}
}