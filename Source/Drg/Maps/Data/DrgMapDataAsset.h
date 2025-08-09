// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgMapDataAsset.generated.h"

/**
 * 게임의 주요 레벨(맵)들에 대한 참조를 관리하는 데이터 에셋입니다.
 * 게임 흐름을 관리하는 서브시스템에서 이 데이터를 사용하여 레벨을 전환합니다.
 */
UCLASS(BlueprintType, Category = "Drg|Game Data")
class DRG_API UDrgMapDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("DrgMapDataAsset", GetFName());
	}

	/** 메인 메뉴 화면으로 사용될 레벨입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Maps")
	TSoftObjectPtr<UWorld> MainMenuMap;

	/** 실제 인게임 플레이가 이루어지는 기본 레벨입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Maps")
	TSoftObjectPtr<UWorld> InGameMap;
};
