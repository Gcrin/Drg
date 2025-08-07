// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DrgGameStateManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	// 초기 상태
	None,
	// 메인 메뉴
	MainMenu,
	// 로비
	Lobby,
	// 게임 플레이
	InGame,
	// 게임 결과창
	PostGame,
	// 게임 종료
	Quitting
};

UCLASS()
class DRG_API UDrgGameStateManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void ChangeState(EGameFlowState NewState);

	EGameFlowState GetCurrentState() const { return CurrentState;}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void HandleMainMenuState();
	void HandleInGameState();
	void HandleQuittingState();

	EGameFlowState CurrentState = EGameFlowState::None;
};
