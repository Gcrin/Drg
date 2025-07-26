// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAIController.h"
#include "BehaviorTree/BehaviorTree.h"

void ADrgAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}
