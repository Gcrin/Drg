// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DropSystemTypes.generated.h"

class UDrgPickupDataAsset;
class UDropTableDataAsset;

/**
 * @brief 드롭될 개별 아이템의 정보 (스폰할 클래스, 확률, 개수)를 정의
 */
USTRUCT(BlueprintType)
struct FDrgDropItemInfo
{
	GENERATED_BODY()

	// 드롭될 아이템의 정보를 담은 데이터 애셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Drop")
	TSoftObjectPtr<UDrgPickupDataAsset> PickupDataAsset;

	// 이 아이템이 드롭될 확률 (0.0 = 0%, 1.0 = 100%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Drop", meta = (UIMin = "0.0", UIMax = "1.0"))
	float Probability = 0.0f;

	// 드롭에 성공했을 때 스폰될 아이템의 개수 범위 (Min/Max)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Drop")
	FIntPoint DropCount = FIntPoint(1, 1);
};

/**
 * @brief 드롭 아이템 목록을 담는 독립적인 데이터 애셋
 */
UCLASS()
class DRG_API UDropTableDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 이 드롭 테이블이 포함하는 모든 아이템의 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Drop")
	TArray<FDrgDropItemInfo> PossibleDrops;
};

/**
 * @brief 드롭 레지스트리 데이터 에셋
 */
UCLASS()
class DRG_API UDropRegistryDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Drop")
	TMap<FGameplayTag, TSoftObjectPtr<UDropTableDataAsset>> DropRegistryMap;
};

/**
 * @brief 사망 이벤트 발생 시 GameplayMessageSubsystem을 통해 방송될 메시지 구조체
 */
USTRUCT(BlueprintType)
struct FDrgActorDeathMessage
{
	GENERATED_BODY()

	// 가해자 액터
	UPROPERTY(BlueprintReadWrite, Category = "Drg|Drop")
	TObjectPtr<AActor> Instigator = nullptr;

	// 피해자 액터
	UPROPERTY(BlueprintReadWrite, Category = "Drg|Drop")
	TObjectPtr<AActor> Victim = nullptr;
};
