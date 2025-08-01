// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgAbilityStatsByLevel.h"
#include "GameplayAbilitySpec.h"
#include "UDrgAbilityDataAsset.generated.h"

/**
 *  @brief 어빌리티의 주요 데이터를 저장
 *  PrimaryDataAsset = 비동기
 *  DataAsset = 동기
 */
UCLASS(BlueprintType)
class DRG_API UUDrgAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 어빌리티 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display")
	FText AbilityName;
	// 어빌리티 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display", meta = (MultiLine = true))
	FText AbilityDescription;
	// 어빌리티 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Display")
	TObjectPtr<UTexture2D> AbilityIcon;
	// 어빌리티 등장 가중치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Upgrade", meta = (ClampMin = "0.0"))
	float SelectionWeight = 1.0f;
	// 어빌리티 레벨 별 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Upgrade")
	TArray<FDrgAbilityLevelData> AbilityLevelData;
	// 어빌리티 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|GAS")
	TSubclassOf<UGameplayAbility> AbilityClass;

public:
	// 최대 레벨(배열의 원소 개수)을 반환하는 함수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drg|Ability")
	int32 GetMaxLevel() const {return AbilityLevelData.Num();}
	// 지정된 레벨의 데이터를 안전하게 가져오는 헬퍼 함수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drg|Ability")
	bool GetLevelData(int32 Level, FDrgAbilityLevelData& OutData) const
	{
		if (AbilityLevelData.IsValidIndex(Level - 1))
		{
			OutData = AbilityLevelData[Level - 1];
			return true;
		}
		return false;
	}
};
