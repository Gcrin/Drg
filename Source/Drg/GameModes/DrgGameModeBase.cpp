// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/UI/DrgHUD.h"
#include "Engine/Engine.h"

ADrgGameModeBase::ADrgGameModeBase()
{
	// HUD 클래스 설정
	HUDClass = ADrgHUD::StaticClass();

	BossTag = FGameplayTag::RequestGameplayTag(FName("Team.Enemy.Boss"));
}

EGameResult ADrgGameModeBase::EvaluateGameEndCondition(AActor* DeadActor)
{
	if (DeadActor == nullptr)
	{
		return EGameResult::None;
	}

	if (ADrgPlayerCharacter* PlayerCharacter = Cast<ADrgPlayerCharacter>(DeadActor))
	{
		if (PlayerCharacter->IsPlayerControlled())
		{
			return EGameResult::Defeat;
		}
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DeadActor);
	if (ASC && BossTag.IsValid() && ASC->HasMatchingGameplayTag(BossTag))
	{
		return EGameResult::Victory;
	}

	return EGameResult::None;
}
