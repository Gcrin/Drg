// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgMessageTypes.generated.h"

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
 * @brief 게임 상태 변경을 알리는 메시지 구조체
 */
USTRUCT(BlueprintType)
struct FDrgGameStateChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Drg|Message")
	EGameFlowState NewState = EGameFlowState::None;
};