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

UCLASS()
class DRG_API ADrgPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure, Category = "Drg|PlayerStats")
	FGameResultData GetGameResultData() const;

	// 킬 카운트 브로드캐스트
	void UpdateKillCount();
	FOnKillCountChanged OnKillCountChanged;

	int32 GetKillCount() const { return KillCount; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|PlayerState")
	int32 KillCount = 0;

private:
	void OnActorDeath(FGameplayTag Channel, const FDrgActorDeathMessage& Message);
	FGameplayMessageListenerHandle ActorDeathMessageListenerHandle;
};
