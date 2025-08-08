// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DrgGameStateManagerSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "DrgGameStateBase.generated.h"

USTRUCT(BlueprintType)
struct FDrgDeathMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	TObjectPtr<AActor> DeadActor = nullptr;;
};

UCLASS()
class DRG_API ADrgGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void OnDeathMessageReceived(FGameplayTag Channel, const FDrgDeathMessage& Message);
	
	UPROPERTY(ReplicatedUsing = OnRep_GameResult)
	EGameResult CurrentGameResult = EGameResult::None;

	UFUNCTION()
	void OnRep_GameResult();

	FGameplayMessageListenerHandle DeathMessageListenerHandle;
};