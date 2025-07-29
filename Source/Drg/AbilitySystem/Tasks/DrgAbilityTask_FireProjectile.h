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
	/**
	 * @brief 지정된 위치에서 발사체를 한 발 또는 여러 발 발사하는 태스크.
	 * 
	 * @param OwningAbility 이 태스크를 실행하는 어빌리티 (자동으로 설정됨).
	 * @param ProjectileClass 스폰할 발사체의 블루프린트 클래스.
	 * @param DamageEffectClass 발사체가 적에게 적용할 데미지 GameplayEffect 클래스.
	 * @param EffectMultiplier 데미지 계산에 사용될 효과 배율 (1.0 = 100%).
	 * @param SocketName 발사체가 생성될 캐릭터 메시의 소켓 이름. 'None'일 경우 캐릭터의 발밑에서 생성.
	 * @param InitialDelay 첫 발이 발사되기 전까지의 대기 시간 (초).
	 * @param NumberOfProjectiles 발사할 총 발사체의 수.
	 * @param DelayBetweenShots 여러 발을 쏠 경우, 각 발사 사이의 대기 시간 (초).
	 * @param MaxRange				투사체의 최대거리

	 */
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability|Tasks",
		meta = (DisplayName = "Fire Projectile Burst", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UDrgAbilityTask_FireProjectile* FireProjectile(
		UGameplayAbility* OwningAbility,
		TSubclassOf<ADrgProjectile> ProjectileClass,
		TSubclassOf<UGameplayEffect> DamageEffectClass,
		FName SocketName,
		float EffectMultiplier = 1.0f,
		float InitialDelay = 0.0f,
		int32 NumberOfProjectiles = 1,
		float DelayBetweenShots = 0.0f,
		float MaxRange = 1.f
	);

	// 한 발 발사될 때마다 실행
	UPROPERTY(BlueprintAssignable)
	FFireProjectileFiredDelegate OnFired;

	// 모든 발사가 끝났을 때 실행
	UPROPERTY(BlueprintAssignable)
	FFireProjectileFinishedDelegate OnFinished;

protected:
	virtual void Activate() override;

private:
	// 다음 발사를 처리하는 함수
	UFUNCTION()
	void FireNextProjectile();

	TSubclassOf<ADrgProjectile> ProjectileClass;
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	FName SocketName;
	int32 NumberOfProjectiles;
	float InitialDelay;
	float DelayBetweenShots;
	float EffectMultiplier;

	int32 ProjectilesFired;
	FTimerHandle TimerHandle;

	float MaxRange;
	float MoveDistance;
	FTransform StartTransform;

	void SetProjectileMaxRange(ADrgProjectile* pDrgProjectile);
};
