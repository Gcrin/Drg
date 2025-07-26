// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAbilityTask_FireProjectile.h"

#include "Drg/Weapons/DrgProjectile.h"


UDrgAbilityTask_FireProjectile* UDrgAbilityTask_FireProjectile::FireProjectile(UGameplayAbility* OwningAbility,
                                                                               TSubclassOf<ADrgProjectile>
                                                                               InProjectileClass,
                                                                               int32 InNumberOfProjectiles,
                                                                               float InDelayBetweenShots)
{
	UDrgAbilityTask_FireProjectile* Task = NewAbilityTask<UDrgAbilityTask_FireProjectile>(OwningAbility);
	Task->ProjectileClass = InProjectileClass;
	Task->NumberOfProjectiles = FMath::Max(1, InNumberOfProjectiles); // 최소 1발 보장
	Task->DelayBetweenShots = InDelayBetweenShots;
	Task->ProjectilesFired = 0;

	return Task;
}

void UDrgAbilityTask_FireProjectile::Activate()
{
	if (!Ability || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		EndTask();
		return;
	}

	// 첫 발은 즉시 발사
	FireNextProjectile();
}

void UDrgAbilityTask_FireProjectile::FireNextProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgAbilityTask_FireProjectile] : 발사체 블루프린트(ProjectileClass)가 지정되지 않았습니다."));
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		// 로그 추가: 아바타 액터가 없는 경우 확인
		UE_LOG(LogTemp, Error, TEXT("[DrgAbilityTask_FireProjectile] : 아바타 액터가 유효하지 않습니다!"));
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	const FTransform SpawnTransform = AvatarActor->GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	// 충돌 처리 방식을 '항상 스폰'으로 오버라이드합니다.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 발사체 스폰
	if (ADrgProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ADrgProjectile>(
		ProjectileClass, SpawnTransform, SpawnParams))
	{
		ProjectilesFired++;
		OnFired.Broadcast(ProjectilesFired); // 몇 번째 발사인지 신호를 보냄
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgAbilityTask_FireProjectile] : 발사체 스폰에 실패했습니다."));
	}

	// 더 쏠 발사체가 남았는지 확인
	if (ProjectilesFired < NumberOfProjectiles)
	{
		// 딜레이가 있다면 타이머 설정, 없다면 그냥 무시
		if (DelayBetweenShots > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, this, &UDrgAbilityTask_FireProjectile::FireNextProjectile, DelayBetweenShots, false);
		}
		else
		{
			// 딜레이가 0이면 그냥 바로 다음 발사
			FireNextProjectile();
		}
	}
	else
	{
		// 모든 발사가 끝났으면 OnFinished 신호를 보내고 태스크 종료
		OnFinished.Broadcast();
		EndTask();
	}
}
