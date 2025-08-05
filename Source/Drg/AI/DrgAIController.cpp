// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Drg/Character/DrgBaseCharacter.h"

void ADrgAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ADrgBaseCharacter* ControlledCharacter = Cast<ADrgBaseCharacter>(InPawn))
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
			DeadStateTagDelegateHandle = ASC->RegisterGameplayTagEvent(DeadTag, EGameplayTagEventType::NewOrRemoved)
			                                .AddUObject(this, &ADrgAIController::OnPawnStateDead);
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
			ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Dead")),
			                              EGameplayTagEventType::NewOrRemoved)
			   .Remove(DeadStateTagDelegateHandle);
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
