// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "DrgGameStateManagerSubsystem.h"
#include "DrgGameModeBase.generated.h"

class ADrgHUD;

UCLASS()
class DRG_API ADrgGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADrgGameModeBase();
	
	UFUNCTION(BlueprintPure, Category = "Drg|Rule")
	EGameResult EvaluateGameEndCondition(AActor* DeadActor);

private:
	FGameplayTag BossTag;
};
