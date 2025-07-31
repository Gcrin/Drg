// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DrgAbilityData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDrgAbilityData : public FTableRowBase
{
	GENERATED_BODY()

	// 어빌리티 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "이름")
	FString AbilityName;

	// 어빌리티 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "아이콘")
	UTexture2D* AbilityIcon;
	
	// 최대 어빌리티 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "최대 레벨")
	int32 MaxAbilityLevel;

	// 어빌리티 등장 가중치(확률)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"), DisplayName = "가중치")
	float SelectionWeight = 1.0f;
	
	// 어빌리티 BP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "BP_에셋")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;
};
