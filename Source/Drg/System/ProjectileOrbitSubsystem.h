// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectileOrbitSubsystem.generated.h"

class ADrgProjectile;

// 서브시스템이 관리할 각 회전 투사체의 데이터
USTRUCT()
struct FOrbitingProjectileData
{
	GENERATED_BODY()

	// 데이터 유효성을 검사
	bool IsValid() const { return Projectile.IsValid() && OrbitCenter.IsValid(); }

	// 실제 투사체 액터에 대한 포인터
	UPROPERTY()
	TWeakObjectPtr<ADrgProjectile> Projectile;
	// 회전의 중심이 되는 액터
	UPROPERTY()
	TWeakObjectPtr<AActor> OrbitCenter;

	// 궤도 반경
	float CachedOrbitRadius = 0.f;
	// 궤도 속도
	float CachedOrbitSpeed = 0.f;
	// 시계 방향인지
	bool bIsOrbitingClockwise = true;

	// 현재 각도
	float CurrentOrbitAngle = 0.f;
};

UCLASS()
class DRG_API UProjectileOrbitSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// 회전할 투사체를 서브시스템에 등록
	void RegisterOrbitingProjectile(ADrgProjectile* ProjectileToRegister);
	// 투사체를 서브시스템에서 등록 해제
	void UnregisterOrbitingProjectile(ADrgProjectile* ProjectileToUnregister);

protected:
	virtual void Tick(float DeltaTime) override;
	// 성능 분석을 위한 함수
	virtual TStatId GetStatId() const override;

private:
	// 관리할 모든 회전 투사체의 데이터를 담는 배열
	TArray<FOrbitingProjectileData> OrbitingProjectilesData;
};
