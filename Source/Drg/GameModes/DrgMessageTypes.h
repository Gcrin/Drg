// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgMessageTypes.generated.h"

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	None,
	MainMenu,
	InGame,
	Pause,
	PostGame,
	Quitting
};

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	None,
	Victory,
	Defeat,
	Draw
};

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	None,
	Health,
	MaxHealth,
	HealthRegen,
	Stamina,
	MaxStamina,
	StaminaRegen,
	AttackDamage,
	Defense,
	AttackSpeed,
	MoveSpeed,
	Experience,
	MaxExperience,
	CharacterLevel,
	PickupRadius
};

/**
 * @brief 게임 상태 변경을 알리는 메시지 구조체
 */
USTRUCT(BlueprintType)
struct FDrgGameStateChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	EGameFlowState NewState = EGameFlowState::None;

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	EGameResult GameResult = EGameResult::None;
};

/**
 * @brief 플레이어의 어트리뷰트 변경을 알리는 메시지 구조체
 */
USTRUCT(BlueprintType)
struct FDrgAttributeChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	EAttributeType AttributeType = EAttributeType::None;
	
	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	float NewValue = 0.f;
};

/**
 * @brief 피해를 입었을 경우, 발신되는 메시지 구조체
 */
USTRUCT(BlueprintType)
struct FDrgDamageMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	AActor* DamagedActor = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	float DamageAmount = 0.f;
};