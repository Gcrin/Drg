// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameResultData.generated.h"

/**
 * 게임 결과 데이터를 담는 구조체
 */
USTRUCT(BlueprintType)
struct DRG_API FGameResultData
{
	GENERATED_BODY()

public:
	// === 인게임 정보 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	int32 FinalWave = 0; // 최종 웨이브 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	int32 SurvivalMinutes = 0; // 생존 시간 분
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	int32 SurvivalSeconds = 0; // 생존 시간 초
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	int32 KillCount = 0; // 킬 카운트

	// === 캐릭터 스탯 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float Health = 0.0f; // 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float MaxHealth = 0.0f; // 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float Stamina = 0.0f; // 스태미나
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float MaxStamina = 0.0f; // 최대 스태미나
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float Experience = 0.0f; // 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float MaxExperience = 0.0f; // 최대 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float CharacterLevel = 0.f; // 최대 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float HealthRegen = 0.0f; // 체력 리젠
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float StaminaRegen = 0.0f; // 스태미나 리젠
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float AttackDamage = 0.0f; // 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float Defense = 0.0f; // 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float AttackSpeed = 0.0f; // 공격속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float MoveSpeed = 0.0f; // 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|InGameData")
	float PickupRadius = 0.0f; // 아이템 획득 범위

private:
	float SurvivalTimeSeconds = 0.0f; // 생존 시간

public:
	void SetSurvivalTimeSeconds(float NewTime) { SurvivalTimeSeconds = NewTime; }
	void CalculateTime()
	{
		const float SafeSeconds = FMath::Max(0.0f, SurvivalTimeSeconds); // 음수 방지
		SurvivalMinutes = FMath::FloorToInt(SafeSeconds / 60.0f);
		SurvivalSeconds = FMath::FloorToInt(SafeSeconds) % 60;
	}
};