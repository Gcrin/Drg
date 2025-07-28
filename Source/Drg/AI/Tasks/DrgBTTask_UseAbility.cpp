// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBTTask_UseAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

EBTNodeResult::Type UDrgBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIController->GetPawn());
	if (!Asc)
	{
		return EBTNodeResult::Failed;
	}

	// 지정된 클래스와 일치하는 어빌리티를 찾아서 실행
	if (Asc->TryActivateAbilityByClass(AbilityClass))
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
