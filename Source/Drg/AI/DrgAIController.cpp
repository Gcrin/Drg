// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/System/DrgGameplayTags.h"

void ADrgAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ADrgBaseCharacter* ControlledCharacter = Cast<ADrgBaseCharacter>(InPawn))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			DeadStateTagDelegateHandle = ASC->RegisterGameplayTagEvent(DrgGameplayTags::State_Dead,
			                                                           EGameplayTagEventType::NewOrRemoved).AddUObject(
				this, &ADrgAIController::OnPawnStateDead);
		}
	}

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ADrgAIController::OnUnPossess()
{
	if (ADrgBaseCharacter* ControlledCharacter = Cast<ADrgBaseCharacter>(GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			ASC->RegisterGameplayTagEvent(DrgGameplayTags::State_Dead, EGameplayTagEventType::NewOrRemoved).Remove(
				DeadStateTagDelegateHandle);
		}
	}
	Super::OnUnPossess();
}

void ADrgAIController::OnPawnStateDead(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) // 태그가 추가되었을 때
	{
		if (BrainComponent)
		{
			BrainComponent->StopLogic(TEXT("사망 상태 진입"));
		}
	}
}
