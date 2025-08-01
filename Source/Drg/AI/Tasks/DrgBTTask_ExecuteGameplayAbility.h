// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "DrgBTTask_ExecuteGameplayAbility.generated.h"

struct FBTExecuteGameplayAbilityMemory
{
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	FDelegateHandle OnAbilityEndedHandle;
	bool bIsAbilityActive = false;
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerComp;
};

UCLASS()
class DRG_API UDrgBTTask_ExecuteGameplayAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UDrgBTTask_ExecuteGameplayAbility();

	// BT 에디터에서 디자이너가 설정할 어빌리티 태그
	UPROPERTY(EditAnywhere, Category = "GameplayAbility")
	FGameplayTag AbilityTag;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	                            EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	void Cleanup(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
