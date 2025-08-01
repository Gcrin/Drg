// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBTTask_ExecuteGameplayAbility.h"
#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BlackboardComponent.h" // 어빌리티 종료 바인딩을 위해 추가

UDrgBTTask_ExecuteGameplayAbility::UDrgBTTask_ExecuteGameplayAbility()
{
	NodeName = "Execute Gameplay Ability";
	// 태스크가 즉시 종료되지 않고, 어빌리티가 끝날 때까지 대기하도록 설정
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UDrgBTTask_ExecuteGameplayAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTExecuteGameplayAbilityMemory* MyMemory = reinterpret_cast<FBTExecuteGameplayAbilityMemory*>(NodeMemory);
	MyMemory->bIsAbilityActive = false;

	auto* AIController = OwnerComp.GetAIOwner();
	if (!ensure(AIController)) { return EBTNodeResult::Failed; }

	auto* Pawn = AIController->GetPawn();
	if (!ensure(Pawn)) { return EBTNodeResult::Failed; }

	auto* ASI = Cast<IAbilitySystemInterface>(Pawn);
	if (!ensure(ASI)) { return EBTNodeResult::Failed; }

	auto* ASC = ASI->GetAbilitySystemComponent();
	if (!ensure(ASC)) { return EBTNodeResult::Failed; }

	if (!ensure(AbilityTag.IsValid()))
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask '%s'의 AbilityTag가 비어있습니다."), *GetName());
		return EBTNodeResult::Failed;
	}

	MyMemory->ASC = ASC;
	// ExecuteTask에서
	MyMemory->OwnerComp = &OwnerComp;
	// [핵심 수정] AddLambda를 사용하여 델리게이트를 직접 등록
	MyMemory->OnAbilityEndedHandle = ASC->OnAbilityEnded.AddLambda(
		[this, &OwnerComp, NodeMemory](const FAbilityEndedData& EndedData)
		{
			FBTExecuteGameplayAbilityMemory* MyMemory = reinterpret_cast<FBTExecuteGameplayAbilityMemory*>(NodeMemory);

			if (MyMemory && MyMemory->bIsAbilityActive && EndedData.AbilityThatEnded && EndedData.AbilityThatEnded->
				GetAssetTags().HasTag(this->AbilityTag))
			{
				MyMemory->bIsAbilityActive = false;
				// 태스크가 유효한 상태일 때만 종료 함수를 호출하도록 방어 코드 추가
				if (MyMemory->OwnerComp.IsValid())
				{
					FinishLatentTask(*MyMemory->OwnerComp, EBTNodeResult::Succeeded);
				}
			}
		});

	// 어빌리티 활성화 시도
	FGameplayTagContainer TagContainer(AbilityTag);
	if (ASC->TryActivateAbilitiesByTag(TagContainer, true))
	{
		MyMemory->bIsAbilityActive = true;
		return EBTNodeResult::InProgress;
	}

	// 활성화 실패 시 즉시 정리 및 실패 처리
	Cleanup(OwnerComp, NodeMemory);
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UDrgBTTask_ExecuteGameplayAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTExecuteGameplayAbilityMemory* MyMemory = reinterpret_cast<FBTExecuteGameplayAbilityMemory*>(NodeMemory);
	if (MyMemory->bIsAbilityActive && MyMemory->ASC.IsValid())
	{
		FGameplayTagContainer TagContainer(AbilityTag);
		MyMemory->ASC->CancelAbilities(&TagContainer);
	}

	Cleanup(OwnerComp, NodeMemory);
	return EBTNodeResult::Aborted;
}

void UDrgBTTask_ExecuteGameplayAbility::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                       EBTNodeResult::Type TaskResult)
{
	Cleanup(OwnerComp, NodeMemory);
}

uint16 UDrgBTTask_ExecuteGameplayAbility::GetInstanceMemorySize() const
{
	return sizeof(FBTExecuteGameplayAbilityMemory);
}

void UDrgBTTask_ExecuteGameplayAbility::Cleanup(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTExecuteGameplayAbilityMemory* MyMemory = reinterpret_cast<FBTExecuteGameplayAbilityMemory*>(NodeMemory);
	if (MyMemory->ASC.IsValid() && MyMemory->OnAbilityEndedHandle.IsValid())
	{
		MyMemory->ASC->OnAbilityEnded.Remove(MyMemory->OnAbilityEndedHandle);
	}
	MyMemory->OnAbilityEndedHandle.Reset();
}
