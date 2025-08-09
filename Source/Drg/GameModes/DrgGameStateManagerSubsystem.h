// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DrgGameStateManagerSubsystem.generated.h"

class UDrgMapDataAsset;

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	None,
	MainMenu,
	InGame,
	PostGame,
	Quitting
};

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	None,
	Victory,
	Defeat,
	Draw
};

/**
 * 게임의 전체적인 흐름과 상태를 관리하는 서브시스템
 */
UCLASS(Config = Game)
class DRG_API UDrgGameStateManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 게임 시작 (엔트리 포인트)
	UFUNCTION(BlueprintCallable, Category = "Drg|State")
	void StartGame();

	// 상태 변경
	UFUNCTION(BlueprintCallable, Category = "Drg|State")
	void ChangeState(EGameFlowState NewState);

	// 게임 결과와 함께 상태 변경
	UFUNCTION(BlueprintCallable, Category = "Drg|State")
	void ChangeStateWithResult(EGameFlowState NewState, EGameResult GameResult = EGameResult::None);

	// 현재 상태 조회 
	UFUNCTION(BlueprintPure, Category = "Drg|State")
	EGameFlowState GetCurrentState() const { return CurrentState; }

	// 현재 게임 결과 조회
	UFUNCTION(BlueprintPure, Category = "Drg|State")
	EGameResult GetCurrentGameResult() const { return CurrentGameResult; }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	// 맵 정보가 담긴 데이터 에셋 경로 (DefaultGame.ini에서 설정)
	UPROPERTY(Config)
	TSoftObjectPtr<UDrgMapDataAsset> MapDataAssetPath;

private:
	// 맵 데이터 에셋 로드
	void OnMapDataLoaded();

	// === 내부 상태 관리 ===
	EGameFlowState CurrentState = EGameFlowState::None;
	EGameResult CurrentGameResult = EGameResult::None;

	// 로드된 맵 데이터 에셋
	UPROPERTY()
	TObjectPtr<const UDrgMapDataAsset> LoadedMapDataAsset;

	// 상태 변경 처리
	void HandleStateChange();

	// 각 상태별 처리 함수들
	void OpenMainMenu();
	void OpenInGameLevel();
	void ShowPostGameResults();
	void QuitGame();
};
