// Fill out your copyright notice in the Description page of Project Settings.

#include "Drg/AbilitySystem/Abilities/DrgOrbitalMovementComponent.h"
#include "Drg/Weapons/DrgProjectile.h"

// Sets default values for this component's properties
UDrgOrbitalMovementComponent::UDrgOrbitalMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDrgOrbitalMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDrgOrbitalMovementComponent::UpdateAllProjectilePositions()
{
	const int32 ProjectileCount = OrbitingProjectiles.Num();
	if (ProjectileCount == 0)
		return;

	const FVector ComponentWorldLocation = GetOwner()->GetActorLocation();

	// 투사체들이 원형으로 균일하게 분포하기 위한 각도 간격입니다.
	const float AngleStep = 360.0f / ProjectileCount;

	for (int32 i = ProjectileCount - 1; i >= 0; --i)
	{
		TWeakObjectPtr<ADrgProjectile> ProjectilePtr = OrbitingProjectiles[i];

		// 투사체가 중간에 파괴되었을 수 있으므로 유효성을 검사합니다.
		if (!ProjectilePtr.IsValid())
		{
			// 유효하지 않은 포인터는 목록에서 제거합니다.
			OrbitingProjectiles.RemoveAt(i);
			continue;
		}

		ADrgProjectile* Projectile = ProjectilePtr.Get();

		// 이 투사체가 위치해야 할 목표 각도를 계산합니다.
		const float TargetAngle = CurrentAngle + (AngleStep * i);

		// 삼각함수를 사용하여 원형 궤도 상의 새 위치를 계산합니다. (X, Y 평면 기준)
		const float NewRelativeX = OrbitRadius * FMath::Cos(FMath::DegreesToRadians(TargetAngle));
		const float NewRelativeY = OrbitRadius * FMath::Sin(FMath::DegreesToRadians(TargetAngle));

		// Z축 위치는 변경하지 않고 X, Y 위치만 업데이트합니다.
		const FVector NewRelativeLocation(NewRelativeX, NewRelativeY, 0.0f);

		// 컴포넌트의 월드 위치에 계산된 상대 위치를 더하여 투사체의 최종 월드 위치를 결정합니다.
		const FVector NewWorldLocation = ComponentWorldLocation + NewRelativeLocation;

		// 투사체의 월드 위치를 직접 설정합니다.
		// Z축 위치는 기존 투사체의 Z 위치를 유지합니다.
		Projectile->SetActorLocation(FVector(NewWorldLocation.X, NewWorldLocation.Y, Projectile->GetActorLocation().Z));
	}
}


// Called every frame
void UDrgOrbitalMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// 회전시킬 투사체가 없으면 아무것도 하지 않습니다.
	if (OrbitingProjectiles.Num() == 0)
	{
		return;
	}

	// 현재 각도를 시간에 따라 업데이트합니다.
	CurrentAngle += RotationSpeed * DeltaTime;
	// 각도가 360도를 넘어가면 0으로 되돌려 오버플로우를 방지합니다.
	if (CurrentAngle > 360.0f)
	{
		CurrentAngle -= 360.0f;
	}

	// 모든 투사체의 위치를 업데이트합니다.
	UpdateAllProjectilePositions();
}

void UDrgOrbitalMovementComponent::AddProjectile(ADrgProjectile* ProjectileToAdd)
{
	if (!IsValid(ProjectileToAdd))
	{
		return;
	}

	// 중복 추가를 방지합니다.
	if (OrbitingProjectiles.Contains(ProjectileToAdd))
	{
		return;
	}

	OrbitingProjectiles.Add(ProjectileToAdd);

	// 새로운 투사체가 추가되었으므로, 모든 투사체의 간격을 재조정합니다.
	UpdateAllProjectilePositions();
}

void UDrgOrbitalMovementComponent::RemoveProjectile(ADrgProjectile* ProjectileToRemove)
{
	if (!(ProjectileToRemove))
	{
		return;
	}

	OrbitingProjectiles.Remove(ProjectileToRemove);
}
