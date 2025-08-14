// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgEvolutionData.h"
#include "DrgUpgradeChoice.generated.h"

UENUM(BlueprintType)
enum class EChoiceType : uint8
{
	Upgrade,
	Evolution
};

/**
 * @brief 레벨 업 선택지(UI, 로직)에 들어갈 데이터들의 구조체
 * Default 값: AbilityData = nullptr, bIsUpgrade = false, 그 외 0
 */
USTRUCT(BlueprintType)
struct FDrgUpgradeChoice
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	TObjectPtr<UDrgAbilityDataAsset> AbilityData = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	bool bIsUpgrade = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	int32 PreviousLevel = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	int32 NextLevel = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	EChoiceType ChoiceType = EChoiceType::Upgrade;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	FDrgEvolutionRecipe EvolutionRecipe;
};
