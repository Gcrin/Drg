// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/UI/DrgHUD.h"
#include "Engine/Engine.h"

ADrgGameModeBase::ADrgGameModeBase()
{
	// HUD 클래스 설정
	HUDClass = ADrgHUD::StaticClass();
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

	// if (Cast<보스>(DeadActor))
	// {
	// 	return EGameResult::Victory;
	// }

	return EGameResult::None;
}
