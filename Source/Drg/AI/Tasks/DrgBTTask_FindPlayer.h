// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "DrgBTTask_FindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class DRG_API UDrgBTTask_FindPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UDrgBTTask_FindPlayer(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|AI", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActorKey;
};
