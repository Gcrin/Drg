// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Paper2D/Classes/PaperSprite.h"
#include "DrgAbilityLevelData.generated.h"

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	Ability UMETA(DisplayName = "Ability"),
	Effect UMETA(DisplayName = "Effect")
};

/**
 *  @brief 레벨 별 업그레이드 데이터 구조체, (Index 0 = Level 1)
 */
USTRUCT(BlueprintType)
struct FDrgAbilityLevelData
{
	GENERATED_BODY()

	// 업그레이드 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Upgrade|Display", meta = (DisplayName = "업그레이드 타입"))
	EUpgradeType UpgradeType;
	// 업그레이드 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display", meta = (MultiLine = true, DisplayName = "설명"))
	FText AbilityDescription;
	// 업그레이드 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display", meta = (DisplayName = "아이콘"))
	TSoftObjectPtr<UPaperSprite> AbilityIcon;
	// 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability", meta = (DisplayName = "어빌리티 클래스",
		EditCondition = "UpgradeType == EUpgradeType::Ability", EditConditionHides))
	TSubclassOf<UGameplayAbility> AbilityClass;
	// 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Effect", meta = (DisplayName = "이펙트 클래스",
		EditCondition = "UpgradeType == EUpgradeType::Effect", EditConditionHides))
	TSubclassOf<UGameplayEffect> EffectClass;
};
