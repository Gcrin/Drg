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
	const float AngleStep = 360.0f / ProjectileCount;

	// 매번 맵을 비워서 최신 상태로 유지합니다.
	TargetLocations.Empty();

	for (int32 i = ProjectileCount - 1; i >= 0; --i)
	{
		TWeakObjectPtr<ADrgProjectile> ProjectilePtr = OrbitingProjectiles[i];

		if (!ProjectilePtr.IsValid())
		{
			OrbitingProjectiles.RemoveAt(i);
			continue;
		}

		ADrgProjectile* Projectile = ProjectilePtr.Get();

		const float TargetAngle = CurrentAngle + (AngleStep * i);

		const float NewRelativeX = OrbitRadius * FMath::Cos(FMath::DegreesToRadians(TargetAngle));
		const float NewRelativeY = OrbitRadius * FMath::Sin(FMath::DegreesToRadians(TargetAngle));
		const FVector NewRelativeLocation(NewRelativeX, NewRelativeY, 0.0f);

		const FVector NewWorldLocation = ComponentWorldLocation + NewRelativeLocation;

		// 투사체의 목표 위치를 맵에 저장합니다.
		TargetLocations.Add(ProjectilePtr,
		                    FVector(NewWorldLocation.X, NewWorldLocation.Y, Projectile->GetActorLocation().Z));
	}
}


// Called every frame
void UDrgOrbitalMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OrbitingProjectiles.Num() == 0)
	{
		return;
	}

	CurrentAngle += RotationSpeed * DeltaTime;
	if (CurrentAngle > 360.0f)
	{
		CurrentAngle -= 360.0f;
	}

	// 매 틱마다 목표 위치를 업데이트합니다.
	UpdateAllProjectilePositions();

	// 이제 각 투사체를 목표 위치로 부드럽게 보간하여 이동시킵니다.
	for (TWeakObjectPtr<ADrgProjectile> ProjectilePtr : OrbitingProjectiles)
	{
		if (ProjectilePtr.IsValid())
		{
			ADrgProjectile* Projectile = ProjectilePtr.Get();

			if (FVector* TargetLocationPtr = TargetLocations.Find(ProjectilePtr))
			{
				const FVector CurrentLocation = Projectile->GetActorLocation();
				// VInterpTo 함수로 현재 위치에서 목표 위치로 부드럽게 보간
				const float InterpolationSpeed = 10.0f; // 이 값을 조절하여 부드러움을 변경할 수 있습니다.
				const FVector NewLocation = FMath::VInterpTo(CurrentLocation, *TargetLocationPtr, DeltaTime,
				                                             InterpolationSpeed);

				Projectile->SetActorLocation(NewLocation);
			}
		}
	}
}

void UDrgOrbitalMovementComponent::AddProjectile(ADrgProjectile* ProjectileToAdd)
{
	if (!IsValid(ProjectileToAdd))
	{
		return;
	}
	// TWeakObjectPtr 변수를 임시로 선언하여 사용합니다.
	TWeakObjectPtr<ADrgProjectile> ProjectileWeakPtr(ProjectileToAdd);

	// 중복 추가를 방지합니다.
	if (OrbitingProjectiles.Contains(ProjectileToAdd))
	{
		return;
	}

	OrbitingProjectiles.Add(ProjectileToAdd);

	// 새로운 투사체가 추가되었으므로, 모든 투사체의 간격을 재조정합니다.
	UpdateAllProjectilePositions();

	if (FVector* TargetLocationPtr = TargetLocations.Find(ProjectileWeakPtr))
	{
		ProjectileToAdd->SetActorLocation(*TargetLocationPtr);
	}
}

void UDrgOrbitalMovementComponent::RemoveProjectile(ADrgProjectile* ProjectileToRemove)
{
	if (!(ProjectileToRemove))
	{
		return;
	}

	OrbitingProjectiles.Remove(ProjectileToRemove);
}
