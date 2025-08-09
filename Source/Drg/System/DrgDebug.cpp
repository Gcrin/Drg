// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgDebug.h"

#if ENABLE_DRAW_DEBUG

TAutoConsoleVariable<int32> CVarDrgDebugOverlapQueries(
	TEXT("Drg.Debug.OverlapQueries"),
	0,
	TEXT("구체 오버랩 쿼리의 디버그 범위를 표시합니다.\n0: 비활성화, 1: 활성화"),
	ECVF_Cheat
);

TAutoConsoleVariable<float> CVarDrgDebugDefaultLifeTime(
	TEXT("Drg.Debug.DefaultLifeTime"),
	1.0f, // 기본 지속 시간
	TEXT("디버그 드로잉이 화면에 표시될 기본 시간을 초 단위로 설정합니다."),
	ECVF_Cheat
);

void DrgDebug::DrawDebugSphereWithCVarCheck(
	const UWorld* World,
	const TAutoConsoleVariable<int32>& DebugCVar,
	const FVector& Center,
	float Radius,
	int32 Segments,
	const FColor& Color,
	bool bPersistentLines,
	uint8 DepthPriority,
	float Thickness)
{
	if (DebugCVar.GetValueOnGameThread() > 0)
	{
		const float LifeTime = CVarDrgDebugDefaultLifeTime.GetValueOnGameThread();

		::DrawDebugSphere(
			World,
			Center,
			Radius,
			Segments,
			Color,
			bPersistentLines,
			LifeTime,
			DepthPriority,
			Thickness
		);
	}
}

#endif
