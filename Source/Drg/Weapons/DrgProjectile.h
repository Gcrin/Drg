// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "DrgProjectile.generated.h"

class UPointLightComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EImpactRotationMethod : uint8
{
	// 충돌 지점의 표면 각도에 맞춰 회전합니다.
	AlignToImpactNormal,
	// 투사체가 날아가던 방향을 유지합니다.
	AlignToProjectile,
	// 항상 기본 회전값(0,0,0)을 사용합니다.
	ZeroRotation
};

USTRUCT(BlueprintType)
struct FDrgProjectileParams
{
	GENERATED_BODY()

	// true이면 포물선 궤적으로 발사합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc")
	bool bEnableArc = false;

	// 포물선의 높이를 조절합니다. (0.0 ~ 1.0 사이 값 권장)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc",
	meta = (EditCondition = "bEnableArc", EditConditionHides, ClampMin = "0.0", UIMin = "0.0"))
	float ArcHeightRatio = 0.5f;

	// 포물선 계산에 사용될 상대적인 목표 위치입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Arc",
		meta = (EditCondition = "bEnableArc", EditConditionHides))
	FVector TargetOffset;

	// true이면 비행 중 주변의 적을 탐지하고 유도합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing")
	bool bEnableHoming = false;

	// 유도 시 타겟을 향해 꺾는 가속도의 크기입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing",
		meta = (EditCondition = "bEnableHoming", EditConditionHides))
	float HomingAcceleration = 5000.f;

	// 유도 기능이 켜졌을 때, 적을 탐지하는 최대 반경입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Homing",
		meta = (EditCondition = "bEnableHoming", EditConditionHides))
	float DetectionRadius = 1000.f;

	// true이면 관통 횟수 제한 없이 무한으로 관통합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Pierce")
	bool bInfinitePierce = false;

	// 이 투사체가 파괴되기 전까지 피해를 입힐 수 있는 최대 대상의 수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Pierce",
		meta=(EditCondition = "!bInfinitePierce", ClampMin = "1", UIMin = "1"))
	int32 MaxTargetHits = 1;

	// true이면 충돌 시 폭발하여 주변에 범위 피해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|AoE")
	bool bEnableAoeOnImpact = false;

	// 범위 피해가 적용될 반경 (cm 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|AoE",
		meta = (EditCondition = "bEnableAoeOnImpact", EditConditionHides))
	float AoeRadius = 100.f;

	// true이면 최초 충돌 대상에게는 기본 피해와 범위 피해를 모두 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|AoE",
		meta = (EditCondition = "bEnableAoeOnImpact", EditConditionHides))
	bool bApplyBaseDamageToInitialTarget = false;

	// 발사 시 재생할 사운드입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Muzzle")
	TObjectPtr<USoundBase> MuzzleSound;

	// 발사 시 스폰할 나이아가라 이펙트입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Muzzle")
	TObjectPtr<UNiagaraSystem> MuzzleVFX;

	// 발사 이펙트의 크기를 조절합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Muzzle",
		meta = (EditCondition = "MuzzleVFX != nullptr"))
	FVector MuzzleScale = FVector::OneVector;

	// 충돌 시 재생할 사운드입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	// 충돌 시 스폰할 나이아가라 이펙트입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Impact")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	// 충돌 이펙트의 크기를 조절합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Impact",
		meta = (EditCondition = "ImpactVFX != nullptr"))
	FVector ImpactScale = FVector::OneVector;

	// 충돌 이펙트의 초기 회전 방식을 결정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Impact",
		meta = (EditCondition = "ImpactVFX != nullptr"))
	EImpactRotationMethod RotationMethod = EImpactRotationMethod::AlignToImpactNormal;

	// 충돌 지점으로부터 이펙트가 얼마나 떨어져서 스폰될지 결정합니다. (cm 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Projectile|Effects|Impact",
		meta = (EditCondition = "ImpactVFX != nullptr"))
	float ImpactOffset = 0.0f;
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

	void SetDamageEffectSpec(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle);
	void SetAoeDamageEffectSpec(const FGameplayEffectSpecHandle& InAoeDamageEffectSpecHandle);


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void StartProjectileArc();
	void DetectTarget();

	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FDrgProjectileParams ProjectileParams;

	// 여러 문제로 인해 블루프린트에서 생성하고 연결
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drg|Components")
	TObjectPtr<UNiagaraComponent> TrailComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UPointLightComponent> PointLightComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	FTransform StartTransform = FTransform();

	virtual void DestroyProjectile();

private:
	// 충돌 처리 로직을 총괄하는 함수
	void ProcessImpact(const FHitResult& HitResult, bool bFromSweep);
	// 충돌 이펙트와 사운드를 재생하는 함수
	void PlayImpactEffects(const FVector& Location, const FRotator& Rotation);

	// 범위 피해를 계산하고 적용하는 함수
	void ExecuteAoeDamage(const FVector& ImpactCenter);

	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	FGameplayEffectSpecHandle AoeDamageEffectSpecHandle;

	EProjectileState ProjectileState;
	FTimerHandle DetectTargetTimerHandle;
	TWeakObjectPtr<AActor> HomingTarget;
	FGameplayTag OwnerTeamTag;
	// 이미 피해를 입힌 액터들을 저장하는 배열
	TArray<TObjectPtr<AActor>> DamagedActors;
};
