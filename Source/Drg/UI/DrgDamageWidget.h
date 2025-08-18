// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DrgDamageWidget.generated.h"

class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class DRG_API UDrgDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|HUD|Damage")
	void SetDamageText(float DamageAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Drg|HUD|Damage")
	UWidgetAnimation* GetFadeAndRiseAnimation() const;
};
