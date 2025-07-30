// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgWaveTableRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)

struct FDrgWaveTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:

	// 웨이브 단계
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveNumber;

	// 스폰할 AI데이터에셋, 스폰 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TObjectPtr<class UDrgCharacterData>, float> AIData;

	// 최대 스폰할 AI 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSpawnCount;

	// 한번에 스폰할 AI 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount;

	// 스폰 시간 간격 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval;
};
