// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DrgBossWidget.generated.h"

UCLASS()
class DRG_API UDrgBossWidget : public UUserWidget
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> BossName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> BossHealth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHealthBar;

	UFUNCTION(BlueprintImplementableEvent, Category = "Drg|Boss")
	void UpdateBossHealth(float NewHealth);
};
