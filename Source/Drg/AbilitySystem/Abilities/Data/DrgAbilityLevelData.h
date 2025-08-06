// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgAbilityLevelData.generated.h"

/**
 *  @brief 어빌리티의 레벨 별 스텟 구조체, (Index 0 = Level 1)
 */
USTRUCT(BlueprintType)
struct FDrgAbilityLevelData
{
	GENERATED_BODY()

	// 어빌리티 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Stats", meta = (DisplayName = "데미지"))
	float Damage = 0.0f;
	// 어빌리티 쿨타임
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Stats", meta = (DisplayName = "쿨타임"))
	float CooldownDuration = 1.0f;
	// 어빌리티 투사체 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Stats", meta = (DisplayName = "투사체 수"))
	int32 ProjectileCount = 1;
	// 어빌리티 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Stats", meta = (DisplayName = "효과 범위"))
	float AreaOfEffect = 100.0f;
};
