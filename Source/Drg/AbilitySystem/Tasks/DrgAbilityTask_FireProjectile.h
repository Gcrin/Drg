// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Drg/Weapons/DrgProjectile.h"
#include "DrgAbilityTask_FireProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireProjectileFinishedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFireProjectileFiredDelegate, int32, ProjectileNumber);

class ADrgProjectile;

USTRUCT(BlueprintType)
struct FDrgFireProjectileParams
{
	GENERATED_BODY()
	// 이 태스크를 실행하는 어빌리티 (자동으로 설정됨).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADrgProjectile> ProjectileClass;

	// 발사체가 적에게 적용할 데미지 GameplayEffect 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 발사체가 생성될 캐릭터 메시의 소켓 이름. 'None'일 경우 캐릭터의 발밑에서 생성.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	// 발사체 트랜스폼
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform ProjectileLocalTransform = FTransform();

	// 데미지 계산에 사용될 효과 배율 (1.0 = 100%).
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.01", UIMin = "0.01", EditCondition = "DamageEffectClass != nullptr"))
	float DirectDamageMultiplier = 1.0f;

	// 범위 데미지 계산에 사용될 효과 배율 (1.0 = 100%).
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.01", UIMin = "0.01", EditCondition = "DamageEffectClass != nullptr"))
	float AoeDamageMultiplier = 1.0f;

	// 첫 발이 발사되기 전까지의 대기 시간 (초).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDelay = 0.0f;

	// 발사할 총 발사체의 수.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1"))
	int32 NumberOfProjectiles = 1;

	// 여러 발을 쏠 경우, 각 발사 사이의 대기 시간 (초).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float DelayBetweenShots = 0.1f;

	// 투사체가 파괴되기까지의 시간(초). 사거리 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float ProjectileLifeSpan = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDrgProjectileParams ProjectileParams;
};

UCLASS()
class DRG_API UDrgAbilityTask_FireProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	// 지정된 위치에서 발사체를 한 발 또는 여러 발 발사하는 태스크.
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability|Tasks",
		meta = (DisplayName = "Fire Projectile Burst", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))

	static UDrgAbilityTask_FireProjectile* FireProjectile(
		UGameplayAbility* OwningAbility,
		const FDrgFireProjectileParams& Params
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

	bool CalculateSpawnTransform(ACharacter* Character, FTransform& OutSpawnTransform);

	FDrgFireProjectileParams Params;

	int32 ProjectilesFired;
	FTimerHandle TimerHandle;

	//발사체 최종 트랜스폼
	FTransform SpawnWorldTransform;
};
