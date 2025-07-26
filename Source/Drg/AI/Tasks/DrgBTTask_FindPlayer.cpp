// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBTTask_FindPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UDrgBTTask_FindPlayer::UDrgBTTask_FindPlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Player");
}

EBTNodeResult::Type UDrgBTTask_FindPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetAIOwner())
	{
		if (UBlackboardComponent* const Blackboard = OwnerComp.GetBlackboardComponent())
		{
			if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
			{
				Blackboard->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerPawn);
                
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
    
	return EBTNodeResult::Failed;
}
