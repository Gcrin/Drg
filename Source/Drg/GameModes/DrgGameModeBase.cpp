// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameModeBase.h"

#include "Drg/Player/DrgPlayerCharacter.h"

EGameResult ADrgGameModeBase::EvaluateGameEndCondition(AActor* DeadActor)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DeadActor->GetClass()->GetName());
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
