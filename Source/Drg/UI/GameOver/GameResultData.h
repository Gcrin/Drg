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
	FGameResultData()
	{
		MonstersKilled = 0;
		GoldEarned = 0;
		SurvivalTimeSeconds = 0.0f;
		FinalLevel = 1;
	}

	// 처치한 몬스터 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Result")
	int32 MonstersKilled;

	// 획득한 골드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Result")
	int32 GoldEarned;

	// 생존 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Result")
	float SurvivalTimeSeconds;

	// 최종 레벨 또는 웨이브
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Result")
	int32 FinalLevel;

	// 생존 시간을 MM:SS 형식으로 변환하는 함수
	FString GetFormattedSurvivalTime() const
	{
		int32 Minutes = FMath::FloorToInt(SurvivalTimeSeconds / 60.0f);
		int32 Seconds = FMath::FloorToInt(SurvivalTimeSeconds) % 60;
		return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	}
};