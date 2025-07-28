// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "DrgBTTask_UseAbility.generated.h"

class UGameplayAbility;
/**
 * 
 */
UCLASS()
class DRG_API UDrgBTTask_UseAbility : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Drg|AI")
	TSubclassOf<UGameplayAbility> AbilityClass;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
