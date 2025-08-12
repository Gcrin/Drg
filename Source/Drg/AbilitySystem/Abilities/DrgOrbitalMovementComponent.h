// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrgOrbitalMovementComponent.generated.h"

class ADrgProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRG_API UDrgOrbitalMovementComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDrgOrbitalMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "OrbitalMovement")
	void AddProjectile(ADrgProjectile* ProjectileToAdd);

	UFUNCTION(BlueprintCallable, Category = "OrbitalMovement")
	void RemoveProjectile(ADrgProjectile* ProjectileToRemove);

protected:
	/** 회전 궤도의 반지름입니다. (cm 단위) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OrbitalMovement|Config")
	float OrbitRadius = 200.0f;

	/** 회전 속도입니다. (초당 회전하는 각도) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OrbitalMovement|Config")
	float RotationSpeed = 180.0f;
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	/** 모든 투사체를 균일한 간격으로 재배치하는 내부 함수입니다. */
	void UpdateAllProjectilePositions();

	UPROPERTY()
	TArray<TWeakObjectPtr<ADrgProjectile>> OrbitingProjectiles;

	/** 현재 회전 각도입니다. 매 프레임 이 값이 업데이트됩니다. */
	float CurrentAngle = 0.0f;

	// 각 투사체의 목표 위치를 저장하는 맵
	TMap<TWeakObjectPtr<ADrgProjectile>, FVector> TargetLocations;
		
};
