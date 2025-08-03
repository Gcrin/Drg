// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DrgMMC_AttackSpeedCooldown.generated.h"

/**
 * 
 */
UCLASS()
class DRG_API UDrgMMC_AttackSpeedCooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UDrgMMC_AttackSpeedCooldown();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
