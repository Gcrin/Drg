// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "Drg/Player/DrgPlayerCharacter.h"

EGameResult ADrgGameModeBase::EvaluateGameEndCondition(AActor* DeadActor)
{
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
