// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgExtraEffects.generated.h"

class UDrgAbilityDataAsset;
/**
 *  더 이상 스킬을 획득할 수 없을 때, 표시되는 이펙트 (체력 회복, 소소한 스탯 상승)
 */
UCLASS()
class DRG_API UDrgExtraEffects : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|ExtraEffects")
	TArray<TObjectPtr<UDrgAbilityDataAsset>> ExtraEffects;
};
