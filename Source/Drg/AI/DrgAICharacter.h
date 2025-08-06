// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "DrgAICharacter.generated.h"

UCLASS()
class DRG_API ADrgAICharacter : public ADrgBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADrgAICharacter();

	virtual void DeactivateCharacter() override;
	virtual void ActivateCharacter() override;
	virtual void OnDeathCleanup() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// AI 컨트롤러를 저장할 변수
	UPROPERTY(Transient)
	TObjectPtr<class ADrgAIController> CachedAIController;
};
