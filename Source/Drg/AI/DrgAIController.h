// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DrgAIController.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class DRG_API ADrgAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void OnPawnStateDead(const FGameplayTag CallbackTag, int32 NewCount);

	FDelegateHandle DeadStateTagDelegateHandle;

	// 사용할 행동 트리 애셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
