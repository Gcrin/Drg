// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Drg/UI/GameOver/GameResultData.h"
#include "GameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DrgPlayerState.generated.h"

struct FDrgActorDeathMessage;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnKillCountChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimeUpdated, float);

UCLASS()
class DRG_API ADrgPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// UI 데이터 제공
	UFUNCTION(BlueprintPure, Category = "Drg|PlayerState")
	FGameResultData GetGameResultData() const;

	// 델리게이트
	FOnKillCountChanged OnKillCountChanged;
	FOnTimeUpdated OnTimeUpdated;

	// 킬 카운트
	void UpdateKillCount();
	int32 GetKillCount() const { return KillCount; }

	// 생존 시간
	UFUNCTION(BlueprintPure, Category = "Drg|PlayerState")
	float GetSurvivalTime() const { return SurvivalTimeSeconds; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|PlayerState")
	int32 KillCount = 0;

private:
	void OnActorDeath(FGameplayTag Channel, const FDrgActorDeathMessage& Message);
	FGameplayMessageListenerHandle ActorDeathMessageListenerHandle;

	// 생존 시간
	FTimerHandle SurvivalTimerHandle;
	float GameStartTime = 0.0f;
	float SurvivalTimeSeconds = 0.0f;
	void UpdateSurvivalTime();
};
