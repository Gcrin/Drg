// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAbilityTask_FireProjectile.h"

#include "AbilitySystemComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgOrbitalMovementComponent.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/Weapons/DrgProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UDrgAbilityTask_FireProjectile* UDrgAbilityTask_FireProjectile::FireProjectile(UGameplayAbility* OwningAbility,
                                                                               const FDrgFireProjectileParams& Params)
{
	UDrgAbilityTask_FireProjectile* Task = NewAbilityTask<UDrgAbilityTask_FireProjectile>(OwningAbility);

	Task->Params = Params;
	Task->Params.NumberOfProjectiles = FMath::Max(1, Params.NumberOfProjectiles); // 최소 1발 보장
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

	if (Params.InitialDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UDrgAbilityTask_FireProjectile::FireNextProjectile,
		                                       Params.InitialDelay, false);
	}
	else
	{
		FireNextProjectile();
	}
}

void UDrgAbilityTask_FireProjectile::FireNextProjectile()
{
	if (!Params.ProjectileClass)
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

	ACharacter* Character = Cast<ACharacter>(AvatarActor);

	if (!Character || !CalculateSpawnTransform(Character, SpawnWorldTransform))
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	UAbilitySystemComponent* SourceASC = Ability->GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC || !Params.DamageEffectClass)
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

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
			Params.ProjectileClass,
			SpawnWorldTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			AvatarActor
		)
	);

	if (SpawnedProjectile)
	{
		// 발사체의 Instigator를 현재 Task의 실행 주체(사용자)로 설정
		SpawnedProjectile->SetInstigator(Cast<APawn>(AvatarActor));
		// 발사체 생명주기
		SpawnedProjectile->InitialLifeSpan = Params.ProjectileLifeSpan;

		// 컨텍스트(Context) 생성
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddInstigator(AvatarActor, SpawnedProjectile);
		ContextHandle.AddSourceObject(AvatarActor);

		// 직격 피해 Spec 생성 및 전달
		FGameplayEffectSpecHandle DirectSpecHandle = SourceASC->MakeOutgoingSpec(
			Params.DamageEffectClass, Ability->GetAbilityLevel(), ContextHandle);
		if (DirectSpecHandle.IsValid())
		{
			DirectSpecHandle.Data->SetSetByCallerMagnitude(DrgGameplayTags::Ability_Multiplier,
			                                               Params.DirectDamageMultiplier);
			SpawnedProjectile->SetDamageEffectSpec(DirectSpecHandle);
		}

		// 범위 피해 Spec 생성 및 전달
		FGameplayEffectSpecHandle AoeSpecHandle = SourceASC->MakeOutgoingSpec(
			Params.DamageEffectClass, Ability->GetAbilityLevel(), ContextHandle);
		if (AoeSpecHandle.IsValid())
		{
			AoeSpecHandle.Data->
			              SetSetByCallerMagnitude(DrgGameplayTags::Ability_Multiplier, Params.AoeDamageMultiplier);
			SpawnedProjectile->SetAoeDamageEffectSpec(AoeSpecHandle);
		}

		// 스폰 완료: 준비된 액터를 월드에 최종적으로 배치.
		UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnWorldTransform);


		// if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		// {
		// 	//플레이어 쪽으로 GA Task의 Param 보내주고 
		// 	Cast<ADrgPlayerCharacter>(PlayerPawn)->AddProjectile(SpawnedProjectile);
		// }
		
		ProjectilesFired++;
		OnFired.Broadcast(ProjectilesFired); // 몇 번째 발사인지 신호를 보냄
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgAbilityTask_FireProjectile] : 발사체 스폰에 실패했습니다."));
	}

	// 더 쏠 발사체가 남았는지 확인
	if (ProjectilesFired < Params.NumberOfProjectiles)
	{
		const float Delay = FMath::Max(0.001f, Params.DelayBetweenShots);
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle, this, &UDrgAbilityTask_FireProjectile::FireNextProjectile, Delay, false);
	}
	else
	{
		// 모든 발사가 끝났으면 OnFinished 신호를 보내고 태스크 종료
		OnFinished.Broadcast();
		EndTask();
	}
}

bool UDrgAbilityTask_FireProjectile::CalculateSpawnTransform(ACharacter* pCharacter, FTransform& OutSpawnTransform)
{
	check(pCharacter);
	USkeletalMeshComponent* pMesh = pCharacter->GetMesh();
	check(pMesh);

	// TODO: 실패 조건- 캐릭터가 발사 불가능한 상태 예)캐릭터 스턴

	FTransform SocketWorldTransform;
	const FRotator CharacterRotation = pCharacter->GetActorRotation();

	if (pMesh->DoesSocketExist(Params.SocketName))
	{
		//SocketWorldTransform = pMesh->GetSocketTransform(Params.SocketName);
		const FVector SocketLocation = pMesh->GetSocketLocation(Params.SocketName);


		SocketWorldTransform = FTransform(CharacterRotation, SocketLocation);
	}
	else
	{
		if (Params.SocketName != NAME_None)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT( "[DrgAbilityTask_FireProjectile] : 캐릭터 '%s'에서 소켓 '%s'를 찾을 수 없습니다. 캐릭터의 기본 위치에서 발사합니다." ),
			       *pCharacter->GetName(), *Params.SocketName.ToString());
		}

		// 소켓이 없을 경우 캐릭터의 위치와 회전을 기준으로
		// 이때 스케일이 커지는 문제를 방지하기 위해 위치와 회전만으로 새로 생성
		SocketWorldTransform = FTransform(CharacterRotation, pCharacter->GetActorLocation());
	}

	OutSpawnTransform = Params.ProjectileLocalTransform * SocketWorldTransform;
	return true;
}

FVector UDrgAbilityTask_FireProjectile::GetSocketLocation(ACharacter* pCharacter) const
{
	// SocketName이 유효한지('None'이 아닌지) 먼저 확인
	if (Params.SocketName != NAME_None && pCharacter && pCharacter->GetMesh())
	{
		// 실제로 그 이름의 소켓이 존재하는지 추가로 확인
		if (pCharacter->GetMesh()->DoesSocketExist(Params.SocketName))
		{
			return pCharacter->GetMesh()->GetSocketLocation(Params.SocketName);
		}

		UE_LOG(LogTemp, Warning, TEXT("[DrgAbilityTask_FireProjectile] : 소켓 '%s'를 찾을 수 없습니다. 기본 위치에서 발사합니다."),
		       *Params.SocketName.ToString());
	}
	// 소켓이 유효하지 않은 경우 기본 위치 사용
	return pCharacter->GetActorLocation();
}
