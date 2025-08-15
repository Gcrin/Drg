// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileOrbitSubsystem.h"

#include "Drg/Weapons/DrgProjectile.h"

void UProjectileOrbitSubsystem::RegisterOrbitingProjectile(ADrgProjectile* ProjectileToRegister)
{
	if (!IsValid(ProjectileToRegister))
	{
		return;
	}

	FOrbitingProjectileData NewData;
	NewData.Projectile = ProjectileToRegister;
	NewData.OrbitCenter = ProjectileToRegister->GetOwner();

	// 발사 시점의 데이터를 캐싱
	NewData.CurrentOrbitAngle = FMath::DegreesToRadians(ProjectileToRegister->GetOrbitStartAngle());
	NewData.CachedOrbitRadius = ProjectileToRegister->GetOrbitRadius();
	NewData.CachedOrbitSpeed = ProjectileToRegister->GetOrbitSpeed();
	NewData.bIsOrbitingClockwise = ProjectileToRegister->IsOrbitClockwise();

	// 회전 중심과 현재 위치를 기준으로 초기 각도를 계산
	/*if (NewData.OrbitCenter.IsValid())
	{
		const FVector DirectionFromCenter = (ProjectileToRegister->GetActorLocation() - NewData.OrbitCenter->
			GetActorLocation()).GetSafeNormal2D();
		NewData.CurrentOrbitAngle = FMath::Atan2(DirectionFromCenter.Y, DirectionFromCenter.X);
	}*/

	// 관리 배열에 추가
	OrbitingProjectilesData.Add(NewData);
}

void UProjectileOrbitSubsystem::UnregisterOrbitingProjectile(ADrgProjectile* ProjectileToUnregister)
{
	if (!IsValid(ProjectileToUnregister))
	{
		return;
	}
	// 조건에 맞는 데이터를 배열에서 제거
	OrbitingProjectilesData.RemoveAll([ProjectileToUnregister](const FOrbitingProjectileData& Data)
	{
		return Data.Projectile == ProjectileToUnregister;
	});
}

void UProjectileOrbitSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 배열을 뒤에서부터 순회하여 제거 시 인덱스 문제를 방지
	for (int32 i = OrbitingProjectilesData.Num() - 1; i >= 0; --i)
	{
		FOrbitingProjectileData& Data = OrbitingProjectilesData[i];

		// 데이터가 유효하지 않으면 (액터가 파괴됨) 배열에서 제거하고 continue
		if (!Data.IsValid())
		{
			OrbitingProjectilesData.RemoveAt(i);
			continue;
		}

		// --- 위치 및 회전 계산 ---
		const float AngularSpeedInRadians = FMath::DegreesToRadians(Data.CachedOrbitSpeed);
		const float DirectionMultiplier = Data.bIsOrbitingClockwise ? -1.f : 1.f;
		Data.CurrentOrbitAngle += AngularSpeedInRadians * DeltaTime * DirectionMultiplier;

		const FVector CenterLocation = Data.OrbitCenter->GetActorLocation();
		const FVector OffsetFromCenter = FVector(FMath::Cos(Data.CurrentOrbitAngle), FMath::Sin(Data.CurrentOrbitAngle),
		                                         0.f) * Data.CachedOrbitRadius;
		const FVector NewLocation = CenterLocation + OffsetFromCenter;

		const FVector TangentDirection = OffsetFromCenter.GetSafeNormal().RotateAngleAxis(
			Data.bIsOrbitingClockwise ? -90.f : 90.f, FVector::UpVector);
		const FRotator NewRotation = TangentDirection.Rotation();

		Data.Projectile->SetActorLocationAndRotation(NewLocation, NewRotation);

		Data.Projectile->AdjustTransformToSurface();
	}
}

TStatId UProjectileOrbitSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UProjectileOrbitSubsystem, STATGROUP_Tickables);
}
