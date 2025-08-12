// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/AbilitySystem/Tasks/DrgAbilityTask_FireProjectile.h"
#include "Engine/DataAsset.h"
#include "DrgFireProjectileDataAsset.generated.h"

UCLASS(BlueprintType)
class DRG_API UDrgFireProjectileParamsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg")
	FDrgFireProjectileParams Params;	
};

UCLASS(BlueprintType)
class DRG_API UDrgAbilityDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 키(Key)는 '어빌리티 레벨', 값(Value)은 해당 레벨의 파라미터 구조체
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg")
	TMap<int32, TObjectPtr<UDrgFireProjectileParamsDataAsset>> LevelParams;

	UFUNCTION(BlueprintCallable, Category = "Drg")
	bool GetParamsForLevel(const int32 InLevel, FDrgFireProjectileParams& OutParams) const
	{
		const TObjectPtr<UDrgFireProjectileParamsDataAsset>* FoundParamsAssetPtr = LevelParams.Find(InLevel);

		if (FoundParamsAssetPtr && *FoundParamsAssetPtr)
		{
			OutParams = (*FoundParamsAssetPtr)->Params;
			return true;
		}
		return false;
	}
	
};
