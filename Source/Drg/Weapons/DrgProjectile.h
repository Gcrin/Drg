// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "DrgProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDrgProjectileParams
{
	GENERATED_BODY()

	/** 이 투사체를 포물선 형태로 발사할지 결정합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc")
	bool bEnableArc = false;

	/** * 포물선의 높이를 조절하는 값입니다. (0.0 ~ 1.0 사이 값 권장)
	 * 값이 높을수록 더 높이 솟아오르는 포물선을 그립니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc",
		meta = (EditCondition = "bEnableArc", EditConditionHides))
	float ArcHeightRatio = 0.5f;

	/** 포물선 발사 시, 발사 위치를 기준으로 할 상대적인 목표 지점입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc",
		meta = (EditCondition = "bEnableArc", EditConditionHides))
	FVector TargetOffset;

	/** 이 투사체가 비행 중 적을 스스로 탐지하여 추적하는 유도 기능를 사용할지 결정합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing")
	bool bEnableHoming = false;

	/**
	 * 유도 기능이 활성화되었을 때, 타겟을 향해 방향을 트는 가속도의 크기입니다.
	 * 값이 높을수록 더 빠르게 타겟을 향해 꺾습니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing",
		meta = (EditCondition = "bEnableHoming", EditConditionHides))
	float HomingAcceleration = 5000.f;

	/** 유도 기능이 활성화되었을 때, 주변의 적을 탐지할 수 있는 최대 반경입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing",
		meta = (EditCondition = "bEnableHoming", EditConditionHides))
	float DetectionRadius = 1000.f;
};

UENUM(BlueprintType)
enum class EProjectileState : uint8
{
	FlyingStraight, // 직선 비행 상태
	Homing // 유도 상태
};

UCLASS()
class DRG_API ADrgProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADrgProjectile();

	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void StartProjectileArc();
	void DetectTarget();

	UPROPERTY(EditAnywhere, Category = "Drg|Parameters")
	FDrgProjectileParams ProjectileParams;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	FTransform StartTransform = FTransform();

	virtual void DestroyProjectile();

private:
	EProjectileState ProjectileState;
	FTimerHandle DetectTargetTimerHandle;
	TWeakObjectPtr<AActor> HomingTarget;

	FGameplayTag OwnerTeamTag;
};
