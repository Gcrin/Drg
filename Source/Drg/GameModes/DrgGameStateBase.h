// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgExperienceManagerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "DrgGameStateBase.generated.h"


UENUM(BlueprintType)
enum class EMatchState : uint8
{
	WaitingToStart UMETA(DisplayName = "WaitingToStart"),
	Inprogress UMETA(DisplayName = "Inprogress"),
	RoundOver UMETA(DisplayName = "RoundOver")
};

UCLASS()
class DRG_API ADrgGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADrgGameStateBase();

	virtual void BeginPlay() override;

	TObjectPtr<UDrgExperienceManagerComponent> ExperienceManagerComponent;
	
	UFUNCTION(BlueprintPure, Category = "MatchState")
	EMatchState GetCurrentMatchState() const;

	void SetMatchState(EMatchState NewState);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	EMatchState CurrentMatchState;
	
	UFUNCTION()
	void OnRep_MatchState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};