// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

#if ENABLE_DRAW_DEBUG

extern TAutoConsoleVariable<int32> CVarDrgDebugOverlapQueries;
extern TAutoConsoleVariable<float> CVarDrgDebugDefaultLifeTime;

namespace DrgDebug
{
	/**
	 * @brief CVar 값을 체크한 후 디버그 구체를 그립니다.
	 * @param World 월드 컨텍스트
	 * @param DebugCVar 체크할 콘솔 변수
	 * @param Center 구체의 중심
	 * @param Radius 구체의 반지름
	 * @param Segments 구체의 부드러운 정도
	 * @param Color 색상
	 * @param bPersistentLines 한 프레임 이상 유지할지 여부
	 * @param DepthPriority 깊이 우선순위 (0: 가려짐, 1 이상: X-ray)
	 * @param Thickness 선 두께
	 */
	void DrawDebugSphereWithCVarCheck(
		const UWorld* World,
		const TAutoConsoleVariable<int32>& DebugCVar,
		const FVector& Center,
		float Radius,
		int32 Segments,
		const FColor& Color,
		bool bPersistentLines = false,
		uint8 DepthPriority = 0,
		float Thickness = 1.0f
	);
}

#endif
