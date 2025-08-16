// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgSkillInformation.generated.h"

class UDrgAbilityDataAsset;

/**
 *  @brief UI에 전달할 보유한 스킬 데이터(이름, 레벨, 진화 여부)
 */
USTRUCT(BlueprintType)
struct FDrgSkillInformation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Drg|SkillInfo")
	TObjectPtr<UDrgAbilityDataAsset> SkillData = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Drg|SkillInfo")
	int32 Level = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Drg|SkillInfo")
	bool bIsEvolution = false;
};
