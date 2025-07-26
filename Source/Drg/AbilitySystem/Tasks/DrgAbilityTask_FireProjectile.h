// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DrgAbilityTask_FireProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireProjectileFinishedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFireProjectileFiredDelegate, int32, ProjectileNumber);

class ADrgProjectile;

UCLASS()
class DRG_API UDrgAbilityTask_FireProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability|Tasks",
		meta = (DisplayName = "Fire Projectile Burst", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UDrgAbilityTask_FireProjectile* FireProjectile(
       UGameplayAbility* OwningAbility,
       TSubclassOf<ADrgProjectile> InProjectileClass,
       FName SocketName,
       int32 InNumberOfProjectiles = 1,
       float InDelayBetweenShots = 0.0f
    );

	UPROPERTY(BlueprintAssignable)
	FFireProjectileFiredDelegate OnFired; // 한 발 발사될 때마다 실행

	UPROPERTY(BlueprintAssignable)
	FFireProjectileFinishedDelegate OnFinished; // 모든 발사가 끝났을 때 실행

protected:
	virtual void Activate() override;

private:
	// 다음 발사를 처리하는 함수
	UFUNCTION()
	void FireNextProjectile();
	
	TSubclassOf<ADrgProjectile> ProjectileClass;
	FName SocketName;
	int32 NumberOfProjectiles;
	float DelayBetweenShots;

	int32 ProjectilesFired;
	FTimerHandle TimerHandle;
};

