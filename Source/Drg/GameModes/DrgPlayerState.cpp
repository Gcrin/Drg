// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerState.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "Drg/Items/Data/DropSystemTypes.h"
#include "Drg/System/DrgGameplayTags.h"

void ADrgPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		ActorDeathMessageListenerHandle = MessageSubsystem.RegisterListener(
			DrgGameplayTags::Event_Broadcast_ActorDied,
			this,
			&ADrgPlayerState::OnActorDeath
		);
	}
}

void ADrgPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ActorDeathMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(ActorDeathMessageListenerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

FGameResultData ADrgPlayerState::GetGameResultData() const
{
	FGameResultData GameResultData;
	GameResultData.KillCount = KillCount;
	
	if (ADrgPlayerCharacter* PlayerCharacter = GetPawn<ADrgPlayerCharacter>())
	{
		if (const UDrgAttributeSet* AttributeSet = PlayerCharacter->GetAttributeSet())
		{
			GameResultData.Health = AttributeSet->GetHealth();
			GameResultData.MaxHealth = AttributeSet->GetMaxHealth();
			GameResultData.HealthRegen = AttributeSet->GetHealthRegen();
			GameResultData.Stamina = AttributeSet->GetStamina();
			GameResultData.MaxStamina = AttributeSet->GetMaxStamina();
			GameResultData.StaminaRegen = AttributeSet->GetStaminaRegen();
			GameResultData.AttackDamage = AttributeSet->GetAttackDamage();
			GameResultData.Defense = AttributeSet->GetDefense();
			GameResultData.AttackSpeed = AttributeSet->GetAttackSpeed();
			GameResultData.MoveSpeed = AttributeSet->GetMoveSpeed();
			GameResultData.Experience = AttributeSet->GetExperience();
			GameResultData.MaxExperience = AttributeSet->GetMaxExperience();
			GameResultData.CharacterLevel = AttributeSet->GetCharacterLevel();
			GameResultData.PickupRadius = AttributeSet->GetPickupRadius();
			// 생존 시간
			// 최종 웨이브 정보
		}
	}
	
	return GameResultData;
}

void ADrgPlayerState::UpdateKillCount()
{
	KillCount++;
	OnKillCountChanged.Broadcast(KillCount);
}

void ADrgPlayerState::OnActorDeath(FGameplayTag Channel, const FDrgActorDeathMessage& Message)
{
	if (!Message.Victim) return;
	if (Message.Victim != GetPawn()) UpdateKillCount();
}