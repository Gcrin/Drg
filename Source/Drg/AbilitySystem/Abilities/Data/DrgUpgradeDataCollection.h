// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgUpgradeDataCollection.generated.h"

class UDrgAbilityDataAsset;

/**
 * @brief 모든 업그레이드 데이터를 저장할 데이터 에셋
 */
UCLASS()
class DRG_API UDrgUpgradeDataCollection : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UDrgAbilityDataAsset>> UpgradeData;
};
