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
	// 이 태스크를 실행하는 어빌리티 (자동으로 설정됨).

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableArc = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (bEnableArc = "true", EditConditionHides))
	float ArcParam = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (bEnableArc = "true", EditConditionHides))
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (bEnableArc = "true", EditConditionHides))
	float VelocityZ = 0.0f;
	


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableChase = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChaseSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChaseDistance = 0.5f;

	// 투사체의 최대거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", UIMin = "0.1"))
	float MaxRange = 1.0f;
};

UCLASS()
class DRG_API ADrgProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADrgProjectile();


	ADrgProjectile(const FDrgProjectileParams& Params);
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 충돌을 감지할 구체 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	float MaxRange = 1.f;
	float MoveDistance = 0.f;
	FTransform StartTransform = FTransform();

	UPROPERTY(EditAnywhere, Category = "Drg|Param")
	FDrgProjectileParams ProjectileParams;
	void CalcDistance();
	void CheckDistance();
	virtual void DestroyProjectile();

	AActor* FindTargetActor();

public:
	void SetMaxRange(float ArgMaxRange);
	void StartProjectileArc();
};
