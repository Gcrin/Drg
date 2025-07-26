// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "Drg/Character/DrgBaseCharacter.h"

void ADrgAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ADrgBaseCharacter* ControlledCharacter = Cast<ADrgBaseCharacter>(InPawn))
	{
		if (ControlledCharacter->GetAttributeSet())
		{
			ControlledCharacter->GetAttributeSet()->OnDeath.AddUObject(this, &ADrgAIController::HandleOnDeath);
		}
	}

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ADrgAIController::HandleOnDeath(AActor* DeadActor)
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("사망"));
	}
}
