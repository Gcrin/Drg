// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DrgExperienceData.generated.h"

USTRUCT(BlueprintType)
struct FDrgExperienceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Level = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxExperience = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SkillPoint = 0;
};