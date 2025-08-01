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
	CachedNodeMemory = NodeMemory;

	FBTExecuteGameplayAbilityMemory* MyMemory = reinterpret_cast<FBTExecuteGameplayAbilityMemory*>(NodeMemory);
	MyMemory->bIsAbilityActive = false;

	auto* AIController = OwnerComp.GetAIOwner();
	check(AIController);
	auto* Pawn = AIController->GetPawn();
	check(Pawn);

	auto* ASI = Cast<IAbilitySystemInterface>(Pawn);
	if (!ensure(ASI)) return EBTNodeResult::Failed;

	auto* ASC = ASI->GetAbilitySystemComponent();
	if (!ensure(ASC)) return EBTNodeResult::Failed;

	if (!ensure(AbilityTag.IsValid()))
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask '%s'의 AbilityTag가 설정되지 않았습니다."), *GetName());
		return EBTNodeResult::Failed;
	}

	MyMemory->ASC = ASC;

	// OwnerComp 저장
	MyMemory->OwnerComp = &OwnerComp;
	// Delegate 바인딩
	FGameplayAbilityEndedDelegate::FDelegate Delegate =
		FGameplayAbilityEndedDelegate::FDelegate::CreateUObject(
			this, &UDrgBTTask_ExecuteGameplayAbility::OnAbilityEnded);
	MyMemory->OnAbilityEndedHandle = ASC->OnAbilityEnded.Add(Delegate);

	FGameplayTagContainer TagContainer(AbilityTag);
	if (ASC->TryActivateAbilitiesByTag(TagContainer, true))
	{
		MyMemory->bIsAbilityActive = true;
		return EBTNodeResult::InProgress;
	}

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

void UDrgBTTask_ExecuteGameplayAbility::OnAbilityEnded(const FAbilityEndedData& EndedData)
{
	if (!CachedNodeMemory) return;

	FBTExecuteGameplayAbilityMemory* MyMemory = (FBTExecuteGameplayAbilityMemory*)CachedNodeMemory;

	if (UBehaviorTreeComponent* OwnerComp = MyMemory->OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
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
