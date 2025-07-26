#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataTable.h"
#include "DrgCharacterStats.generated.h"

USTRUCT(BlueprintType)
struct FAttributeInitializationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;
};

USTRUCT(BlueprintType)
struct FDrgCharacterStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Stats")
	TArray<FAttributeInitializationData> InitialAttributes;
};
