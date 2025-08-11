// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/Character/Data/DrgCharacterData.h"
#include "Drg/System/DrgGameplayTags.h"

void ADrgAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADrgBaseCharacter* ControlledCharacter = Cast<ADrgBaseCharacter>(InPawn);
	if (ControlledCharacter)
	{
		if (UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent())
		{
			DeadStateTagDelegateHandle = ASC->RegisterGameplayTagEvent(DrgGameplayTags::State_Dead,
			                                                           EGameplayTagEventType::NewOrRemoved).AddUObject(
				this, &ADrgAIController::OnPawnStateDead);
		}
	}

	// 캐릭터 데이터 에셋의 비해비어 트리 실행
	UDrgCharacterData* CharacterData = ControlledCharacter->GetCharacterData();
	if (CharacterData && CharacterData->BehaviorTree)
	{
		UBlackboardData* BlackboardAsset = CharacterData->BehaviorTree->BlackboardAsset;
		if (BlackboardAsset)
		{
			// 현재 컨트롤러의 블랙보드 컴포넌트를 새로운 에셋으로 초기화
			if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
			{
				BlackboardComp->InitializeBlackboard(*BlackboardAsset);
			}
		}
		RunBehaviorTree(CharacterData->BehaviorTree);
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
