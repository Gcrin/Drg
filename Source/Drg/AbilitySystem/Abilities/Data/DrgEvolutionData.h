// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgEvolutionData.generated.h"

class UDrgAbilityDataAsset;
class USkeletalMesh;
class UMaterialInterface;

/*
 * @brief 어빌리티, 이펙트의 진화(조합) 재료의 데이터
 */
USTRUCT(BlueprintType)
struct FDrgIngredientInfo
{
	GENERATED_BODY()

	// 진화(조합) 재료
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "재료"))
	TObjectPtr<UDrgAbilityDataAsset> IngredientAsset;
	// 진화(조합) 후 삭제 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "진화(조합) 후 삭제"))
	bool bShouldBeRemoved = true;
};

/*
 * @brief 어빌리티, 이펙트의 조합 레시피와 진화(조합) 스킬
 */
USTRUCT(BlueprintType)
struct FDrgEvolutionRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "진화(조합) 레시피"))
	TArray<FDrgIngredientInfo> Ingredients;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "진화(조합) 스킬"))
	TObjectPtr<UDrgAbilityDataAsset> EvolvedAbilityAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "메시"))
	TObjectPtr<USkeletalMesh> NewMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "머티리얼/ 0 = Face, 1 = Matarial"))
	TArray<TObjectPtr<UMaterialInterface>> NewMaterials;
};

/*
 * @brief 진화(조합) 스킬들을 모아둔 데이터 에셋
 */
UCLASS()
class DRG_API UDrgEvolutionDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Evolution Recipes")
	TArray<FDrgEvolutionRecipe> EvolutionRecipes;
};
