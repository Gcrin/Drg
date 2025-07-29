// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DrgGameConfig.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UDrgGameConfig* GameConfig; // 데이터에셋 // UI, BGM, 설정 등
	
	UFUNCTION(BlueprintPure, Category = "MatchState")
	EMatchState GetCurrentMatchState() const; // Getter

	void SetMatchState(EMatchState NewState); // 게임모드에서 관리

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	EMatchState CurrentMatchState; // 현재 스테이트 상태 // 리플리케이트 대상
	
	UFUNCTION()
	void OnRep_MatchState(); // 복제, 변경 시도되면 호출

	// 네트워크 복제 속성
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
