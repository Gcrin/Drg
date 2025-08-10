// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/Interfaces/DroppableInterface.h"
#include "DrgAICharacter.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIDied, class ADrgAICharacter*, DeadAI);

UCLASS()
class DRG_API ADrgAICharacter : public ADrgBaseCharacter, public IDroppableInterface
{
	GENERATED_BODY()

public:
	ADrgAICharacter();

	virtual FGameplayTag GetDropTag() const override;

	UPROPERTY(BlueprintAssignable, Category = "Pooling")
	FOnAIDied OnAIDied;

	virtual void DeactivateCharacter() override;
	virtual void ActivateCharacter() override;
	virtual void OnDeathCleanup() override;

protected:
	virtual void BeginPlay() override;

	// AI 컨트롤러를 저장할 변수
	UPROPERTY(Transient)
	TObjectPtr<class ADrgAIController> CachedAIController;
};
