// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DrgAbilityLevelData.generated.h"

/**
 *  @brief 레벨 별 어빌리티 데이터 구조체, (Index 0 = Level 1)
 */
USTRUCT(BlueprintType)
struct FDrgAbilityLevelData
{
	GENERATED_BODY()

	// 어빌리티 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display", meta = (MultiLine = true, DisplayName = "설명"))
	FText AbilityDescription;
	// 어빌리티 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display", meta = (DisplayName = "아이콘"))
	TSoftObjectPtr<UTexture2D> AbilityIcon;
	// 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability", meta = (DisplayName = "어빌리티 클래스"))
	TSubclassOf<UGameplayAbility> AbilityClass;
	// 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Effect", meta = (DisplayName = "이펙트 클래스"))
	TSubclassOf<UGameplayEffect> EffectClass;
};
