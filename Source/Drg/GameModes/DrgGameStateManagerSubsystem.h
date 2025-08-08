// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DrgGameStateManagerSubsystem.generated.h"

class UDrgMapDataAsset;

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	// 초기 상태
	None,
	// 메인 메뉴
	MainMenu,
	// 게임 플레이
	InGame,
	// 게임 결과창
	PostGame,
	// 게임 종료
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

UCLASS(Config = Game)
class DRG_API UDrgGameStateManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Drg|State")
	void ChangeState(EGameFlowState NewState);

	UFUNCTION(BlueprintPure, Category = "Drg|State")
	EGameFlowState GetCurrentState() const { return CurrentState; }
	
	void ChangeStateWithResult(EGameFlowState NewState, EGameResult GameResult = EGameResult::None);
	
	EGameResult GetCurrentGameResult() const { return CurrentGameResult; }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 게임모드 등 외부에서 호출할 로딩 시퀀스 시작 함수
	void StartLoadingSequence();

protected:
	// 게임 흐름에 필요한 맵 정보가 담긴 데이터 에셋의 경로입니다.
	UPROPERTY(Config)
	TSoftObjectPtr<UDrgMapDataAsset> MapDataAssetPath;

private:
	void HandleMainMenuState();
	void HandleInGameState();
	void HandlePostGameState();
	void HandleQuittingState();

	// 데이터 로딩이 완료되면 호출되는 함수
	void OnMapDataLoaded();

	EGameFlowState CurrentState = EGameFlowState::None;
	EGameResult CurrentGameResult = EGameResult::None; 

	/**
	 * MapDataAssetPath를 통해 비동기로 로드된 맵 데이터 에셋의 const 포인터입니다.
	 *
	 * OnMapDataLoaded() 콜백 함수에서 유효한 값을 가지게 되며,
	 * 이후 각 게임 상태에 맞는 맵으로 이동할 때 사용됩니다.
	 */
	UPROPERTY()
	TObjectPtr<const UDrgMapDataAsset> LoadedMapDataAsset;
};
