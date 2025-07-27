// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAbilityTask_FireProjectile.h"

#include "AbilitySystemComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgGameplayAbility.h"
#include "Drg/Weapons/DrgProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UDrgAbilityTask_FireProjectile* UDrgAbilityTask_FireProjectile::FireProjectile(UGameplayAbility* OwningAbility,
                                                                               TSubclassOf<ADrgProjectile>
                                                                               InProjectileClass,
                                                                               TSubclassOf<UGameplayEffect>
                                                                               InDamageEffectClass,
                                                                               FName SocketName,
                                                                               int32 InNumberOfProjectiles,
                                                                               float InDelayBetweenShots,
                                                                               float InEffectMultiplier)
{
	UDrgAbilityTask_FireProjectile* Task = NewAbilityTask<UDrgAbilityTask_FireProjectile>(OwningAbility);
	Task->ProjectileClass = InProjectileClass;
	Task->DamageEffectClass = InDamageEffectClass;
	Task->SocketName = SocketName;
	Task->NumberOfProjectiles = FMath::Max(1, InNumberOfProjectiles); // 최소 1발 보장
	Task->DelayBetweenShots = InDelayBetweenShots;
	Task->EffectMultiplier = InEffectMultiplier;
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
		UE_LOG(LogTemp, Error, TEXT("[DrgAbilityTask_FireProjectile] : 아바타 액터가 유효하지 않습니다!"));
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	FVector SpawnLocation;
	ACharacter* Character = Cast<ACharacter>(AvatarActor);

	// SocketName이 유효한지('None'이 아닌지) 먼저 확인
	if (SocketName != NAME_None && Character && Character->GetMesh())
	{
		// 실제로 그 이름의 소켓이 존재하는지 추가로 확인
		if (Character->GetMesh()->DoesSocketExist(SocketName))
		{
			SpawnLocation = Character->GetMesh()->GetSocketLocation(SocketName);
		}
		else
		{
			// 소켓은 지정했지만 오타 등으로 찾지 못한 경우, 경고 로그를 남기고 기본 위치 사용
			UE_LOG(LogTemp, Warning, TEXT("[DrgAbilityTask_FireProjectile] : 소켓 '%s'를 찾을 수 없습니다. 기본 위치에서 발사합니다."),
			       *SocketName.ToString());
			SpawnLocation = AvatarActor->GetActorLocation();
		}
	}
	else
	{
		// 소켓 이름이 지정되지 않은 경우, 기본 위치 사용
		SpawnLocation = AvatarActor->GetActorLocation();
	}

	const FRotator SpawnRotation = AvatarActor->GetActorRotation();
	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	/*
	 * 지연된 스폰(Deferred Spawn)을 사용하는 이유:
	 * 
	 * 일반 SpawnActor 함수는 'AlwaysSpawn' 옵션 사용 시 생성 직후 충돌이 발생하면 스폰은 성공시키지만 nullptr를 반환하는 문제가 있습니다.
	 * 이로 인해 스폰 성공 여부를 안정적으로 확인할 수 없어 무한 발사 버그가 발생했습니다.
	 * 아래의 BeginDeferredActorSpawn / FinishSpawningActor 방식은
	 * 충돌이 발생해도 스폰된 액터의 유효한 포인터를 확실하게 반환해주므로 이 문제를 해결하기 위해 사용합니다.
	 */

	// 스폰 준비: 액터를 월드에 바로 만들지 않고, 추가 설정을 할 수 있도록 메모리에만 준비.
	ADrgProjectile* SpawnedProjectile = Cast<ADrgProjectile>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(
			GetWorld(),
			ProjectileClass,
			SpawnTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			AvatarActor
		)
	);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetInstigator(Cast<APawn>(AvatarActor));
		if (DamageEffectClass)
		{
			UAbilitySystemComponent* SourceASC = Ability->GetAbilitySystemComponentFromActorInfo();
			if (SourceASC)
			{
				FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
				ContextHandle.AddSourceObject(AvatarActor);
				ContextHandle.AddInstigator(AvatarActor, SpawnedProjectile);
				FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
					DamageEffectClass, Ability->GetAbilityLevel(), ContextHandle);

				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(TEXT("Ability.Multiplier")), EffectMultiplier);
					SpawnedProjectile->DamageEffectSpecHandle = SpecHandle;
				}
			}
		}

		// 스폰 완료: 준비된 액터를 월드에 최종적으로 배치.
		UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnTransform);
	}

	// 발사체 스폰 체크
	if (SpawnedProjectile)
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
