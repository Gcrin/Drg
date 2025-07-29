// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "DrgProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UGameplayEffect;

UCLASS()
class DRG_API ADrgProjectile : public AActor
{
	GENERATED_BODY()

public:
	ADrgProjectile();

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

	float MaxRange;
	float MoveDistance;
	FTransform StartTransform;

	void CalcDistance();
	void CheckDistance();
	virtual void DestroyProjectile();

public:
	void SetMaxRange(float ArgMaxRange);
	void SetStartTransform(FTransform ArgStartTransform);
};
