#include "InGameHUDWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DrgBossWidget.h"
#include "DrgHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgUpgradeComponent.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/UI/LevelUp/DrgSkillWidget.h"
#include "Drg/UI/DrgSkillInformation.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "Drg/GameModes/DrgPlayerState.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BossTag = FGameplayTag::RequestGameplayTag(FName("Team.Enemy.Boss"));
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	AttributeChangeMessageListenerHandle = MessageSubsystem.RegisterListener(
		DrgGameplayTags::Event_Broadcast_AttributeChanged,
		this,
		&UInGameHUDWidget::OnAttributeChangedReceived
	);
	DamagedActorMessageListenerHandle = MessageSubsystem.RegisterListener(
		DrgGameplayTags::Event_Broadcast_ActorDamaged,
		this,
		&UInGameHUDWidget::OnDamagedActor
	);

	if (ADrgPlayerState* DrgPlayerState = GetOwningPlayerState<ADrgPlayerState>())
	{
		HandleKillCountChanged(DrgPlayerState->GetKillCount());
		KillCountChangedHandle = DrgPlayerState->OnKillCountChanged.AddUObject(
			this, &UInGameHUDWidget::HandleKillCountChanged);

		HandleTimeUpdated(DrgPlayerState->GetSurvivalTime());
		TimeUpdatedHandle = DrgPlayerState->OnTimeUpdated.AddUObject(this, &UInGameHUDWidget::HandleTimeUpdated);

		HandleWaveNumberChanged(DrgPlayerState->GetCurrentWaveNumber(), DrgPlayerState->GetbIsLastWave());
		WaveNumberChangedHandle = DrgPlayerState->OnWaveNumberChanged.AddUObject(
			this, &UInGameHUDWidget::HandleWaveNumberChanged);
	}

	if (ADrgPlayerCharacter* PlayerCharacter = GetOwningPlayerPawn<ADrgPlayerCharacter>())
	{
		if (UDrgUpgradeComponent* UpgradeComponent = PlayerCharacter->FindComponentByClass<UDrgUpgradeComponent>())
		{
			EquippedSkillsChangedHandle = UpgradeComponent->OnEquippedSkillsChanged.AddUObject(
				this, &UInGameHUDWidget::HandleEquippedSkillsChanged);
			HandleEquippedSkillsChanged();
		}
	}

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (ADrgPlayerCharacter* PlayerCharacter = Cast<ADrgPlayerCharacter>(PlayerController->GetPawn()))
		{
			if (const UDrgAttributeSet* Attributes = PlayerCharacter->GetAttributeSet())
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
}

void UInGameHUDWidget::NativeDestruct()
{
	if (AttributeChangeMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(AttributeChangeMessageListenerHandle);
		MessageSubsystem.UnregisterListener(DamagedActorMessageListenerHandle);
	}
	if (ADrgPlayerState* DrgPlayerState = GetOwningPlayerState<ADrgPlayerState>())
	{
		DrgPlayerState->OnKillCountChanged.Remove(KillCountChangedHandle);
		DrgPlayerState->OnTimeUpdated.Remove(TimeUpdatedHandle);
		DrgPlayerState->OnWaveNumberChanged.Remove(WaveNumberChangedHandle);
	}
	if (ADrgPlayerCharacter* PlayerCharacter = GetOwningPlayerPawn<ADrgPlayerCharacter>())
	{
		if (UDrgUpgradeComponent* UpgradeComponent = PlayerCharacter->FindComponentByClass<UDrgUpgradeComponent>())
		{
			UpgradeComponent->OnEquippedSkillsChanged.Remove(EquippedSkillsChangedHandle);
		}
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

void UInGameHUDWidget::OnDamagedActor(FGameplayTag Channel, const FDrgDamageMessage& Message)
{
	if (Message.DamagedActor && Message.DamagedActor != GetOwningPlayerPawn())
	{
		if (ADrgHUD* HUD = GetOwningPlayer()->GetHUD<ADrgHUD>())
		{
			FVector2D ScreenPosition;
			if (UGameplayStatics::ProjectWorldToScreen(
				GetOwningPlayer(), Message.DamagedActor->GetActorLocation(), ScreenPosition))
			{
				HUD->RequestDamageNumber(Message.DamageAmount, Message.DamagedActor->GetActorLocation());
			}
		}
	}

	if (Message.DamagedActor)
	{
		UAbilitySystemComponent* ASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Message.DamagedActor);
		if (ASC && ASC->HasMatchingGameplayTag(BossTag))
		{
			UWidgetComponent* HealthBarComponent = Message.DamagedActor->FindComponentByClass<UWidgetComponent>();
			if (!BossWidgetClass) return;
			if (!HealthBarComponent)
			{
				HealthBarComponent = NewObject<UWidgetComponent>(Message.DamagedActor);
				if (HealthBarComponent)
				{
					HealthBarComponent->SetWidgetClass(BossWidgetClass);
					HealthBarComponent->SetVisibility(true);
					HealthBarComponent->SetRelativeLocation(FVector(0, 0, 250));
					HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
					HealthBarComponent->SetDrawAtDesiredSize(true);
					HealthBarComponent->RegisterComponent();
					HealthBarComponent->AttachToComponent(
						Message.DamagedActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
			if (HealthBarComponent)
			{
				if (UDrgBossWidget* BossWidgetInstance =
					Cast<UDrgBossWidget>(HealthBarComponent->GetUserWidgetObject()))
				{
					const UDrgAttributeSet* BossAttributes = ASC->GetSet<UDrgAttributeSet>();
					if (BossAttributes)
					{
						float HealthPercentage = BossAttributes->GetHealth() / BossAttributes->GetMaxHealth();
						BossWidgetInstance->UpdateBossHealth(HealthPercentage);
					}
				}
			}
		}
	}
}

void UInGameHUDWidget::HandleKillCountChanged(int32 NewKillCount)
{
	OnUpdateKillCount(NewKillCount);
}

void UInGameHUDWidget::HandleWaveNumberChanged(int32 NewWaveNumber, bool bIsLastWave)
{
	OnWaveNumberChanged(NewWaveNumber, bIsLastWave);
}

void UInGameHUDWidget::HandleEquippedSkillsChanged()
{
	if (!AbilityListBox || !EffectListBox || !SkillWidgetClass) return;

	ADrgPlayerCharacter* PlayerCharacter = GetOwningPlayerPawn<ADrgPlayerCharacter>();
	if (!PlayerCharacter) return;
	UDrgUpgradeComponent* UpgradeComponent = PlayerCharacter->FindComponentByClass<UDrgUpgradeComponent>();
	if (!UpgradeComponent) return;

	const TArray<FDrgSkillInformation> EquippedSkills = UpgradeComponent->GetEquippedSkills();

	AbilityListBox->ClearChildren();
	EffectListBox->ClearChildren();

	TArray<FDrgSkillInformation> AbilityList;
	TArray<FDrgSkillInformation> EffectList;

	for (const auto& Skill : EquippedSkills)
	{
		if (!Skill.SkillData) continue;
		FDrgAbilityLevelData LevelData;
		if (Skill.SkillData->GetLevelData(Skill.Level, LevelData))
		{
			if (LevelData.UpgradeType == EUpgradeType::Ability) AbilityList.Add(Skill);
			else if (LevelData.UpgradeType == EUpgradeType::Effect) EffectList.Add(Skill);
		}
	}

	AbilityListBox->AddChildToHorizontalBox(AbilityTitle);
	for (const auto& Ability : AbilityList)
	{
		if (Ability.bIsEvolution)
		{
			if (UDrgSkillWidget* NewSkillWidget = CreateWidget<UDrgSkillWidget>(this, SkillWidgetClass))
			{
				NewSkillWidget->SetAbilityInfo(Ability.SkillData, Ability.Level, Ability.bIsEvolution);
				AbilityListBox->AddChildToHorizontalBox(NewSkillWidget);
			}
		}
	}
	for (const auto& Ability : AbilityList)
	{
		if (!Ability.bIsEvolution)
		{
			if (UDrgSkillWidget* NewSkillWidget = CreateWidget<UDrgSkillWidget>(this, SkillWidgetClass))
			{
				NewSkillWidget->SetAbilityInfo(Ability.SkillData, Ability.Level, Ability.bIsEvolution);
				AbilityListBox->AddChildToHorizontalBox(NewSkillWidget);
			}
		}
	}

	EffectListBox->AddChildToHorizontalBox(EffectTitle);
	for (const auto& Effect : EffectList)
	{
		if (Effect.bIsEvolution)
		{
			if (UDrgSkillWidget* NewSkillWidget = CreateWidget<UDrgSkillWidget>(this, SkillWidgetClass))
			{
				NewSkillWidget->SetAbilityInfo(Effect.SkillData, Effect.Level, Effect.bIsEvolution);
				EffectListBox->AddChildToHorizontalBox(NewSkillWidget);
			}
		}
	}
	for (const auto& Effect : EffectList)
	{
		if (!Effect.bIsEvolution)
		{
			if (UDrgSkillWidget* NewSkillWidget = CreateWidget<UDrgSkillWidget>(this, SkillWidgetClass))
			{
				NewSkillWidget->SetAbilityInfo(Effect.SkillData, Effect.Level, Effect.bIsEvolution);
				EffectListBox->AddChildToHorizontalBox(NewSkillWidget);
			}
		}
	}
}

void UInGameHUDWidget::HandleTimeUpdated(float SurvivalTimeSeconds)
{
	const int32 Minutes = FMath::FloorToInt(SurvivalTimeSeconds) / 60;
	const int32 Seconds = FMath::FloorToInt(SurvivalTimeSeconds) % 60;
	OnTimerUpdated(Minutes, Seconds);
}

void UInGameHUDWidget::UpdateHealthBar(float Health, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f) HealthBar->SetPercent(Health / MaxHealth);
}

void UInGameHUDWidget::UpdateStaminaBar(float Stamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0.0f) StaminaBar->SetPercent(Stamina / MaxStamina);
}

void UInGameHUDWidget::UpdateExperienceBar(float Exp, float MaxExp)
{
	if (ExperienceBar && MaxExp > 0.0f) ExperienceBar->SetPercent(Exp / MaxExp);
}
