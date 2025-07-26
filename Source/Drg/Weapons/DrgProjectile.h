// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 이 발사체가 적에게 적용할 게임플레이 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	// 충돌을 감지할 구체 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
