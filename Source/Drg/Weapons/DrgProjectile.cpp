// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrgProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/System/DrgDebug.h"
#include "Drg/System/DrgGameplayStatics.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/System/ProjectileOrbitSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADrgProjectile::ADrgProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체인 SphereComponent 설정
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 폰하고만 오버랩 이벤트 발생
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 벽과도 충돌
	SphereComponent->SetSphereRadius(10.f);

	// 발사체 움직임 컴포넌트 설정
	ProjectileMovementComponent = CreateDefaultSubobject<
		UDrgProjectileMovementComponent>(TEXT("DrgProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 1500.f;
	ProjectileMovementComponent->MaxSpeed = 1500.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f; // 중력 영향 안받음
	// 투사체가 속도 방향을 따라 회전
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// 포인트라이트 컴포넌트 설정
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightComponent->SetupAttachment(RootComponent);
	PointLightComponent->SetIntensity(5000.f);
	PointLightComponent->AttenuationRadius = 300.0f;
}

void ADrgProjectile::SetDamageEffectSpec(const FGameplayEffectSpecHandle& InDamageEffectSpecHandle)
{
	DamageEffectSpecHandle = InDamageEffectSpecHandle;
}

void ADrgProjectile::SetAoeDamageEffectSpec(const FGameplayEffectSpecHandle& InAoeDamageEffectSpecHandle)
{
	AoeDamageEffectSpecHandle = InAoeDamageEffectSpecHandle;
}

void ADrgProjectile::ExecuteAoeDamage(const FVector& ImpactCenter)
{
	if (!AoeDamageEffectSpecHandle.IsValid())
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgProjectile::ExecuteAoeDamage] : OwnerActor가 유효하지 않습니다. 범위 피해를 적용할 수 없습니다."));
		return;
	}

	UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!ensure(SourceAsc))
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgProjectile::ExecuteAoeDamage] : SourceAsc가 유효하지 않습니다. 범위 피해를 적용할 수 없습니다."));
		return;
	}

	// SphereOverlap으로 주변 액터를 탐지
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 탐지 무시 목록
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerActor);
	IgnoreActors.Add(this);

	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapActors(
		this,
		ImpactCenter,
		ProjectileParams.AoeRadius,
		ObjectTypes,
		ADrgBaseCharacter::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);

#if ENABLE_DRAW_DEBUG
	DrgDebug::DrawDebugSphereWithCVarCheck(
		GetWorld(),
		CVarDrgDebugOverlapQueries, // 어떤 CVar를 쓸지 지정
		ImpactCenter,
		ProjectileParams.AoeRadius,
		24,
		bHasOverlap ? FColor::Green : FColor::Red
	);
#endif

	for (AActor* TargetActor : OverlappedActors)
	{
		//이미 피해를 받은 액터 제외
		if (!TargetActor) continue;

		UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetAsc)
		{
			// 아군 여부, 사망 여부 등 체크
			if (UDrgGameplayStatics::AreTeamsFriendly(OwnerTeamTag, TargetAsc)) continue;
			ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(TargetActor);
			if (TargetCharacter && TargetCharacter->IsDead()) continue;

			// 데미지 이펙트 적용
			SourceAsc->ApplyGameplayEffectSpecToTarget(*AoeDamageEffectSpecHandle.Data.Get(), TargetAsc);
		}
	}
}

void ADrgProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ProjectileParams.MovementType == EProjectileMovementType::Straight)
	{
		AdjustTransformToSurface();
	}
}

bool ADrgProjectile::AdjustTransformToSurface(float MaxWalkableSlopeAngle)
{
	if (!SphereComponent) return false;

	const float SphereRadius = SphereComponent->GetScaledSphereRadius();
	const FVector CurrentLocation = GetActorLocation();

	const FVector TraceStart = CurrentLocation + FVector(0.f, 0.f, 1000.f);
	const FVector TraceEnd = CurrentLocation - FVector(0.f, 0.f, SphereRadius * 0.1f);

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		TraceStart,
		TraceEnd,
		SphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		{},
		EDrawDebugTrace::None,
		OutHits,
		true
	);

	// 가장 먼저 감지된(가장 높은) 유효한 땅을 찾기
	FHitResult BestGroundHit;
	bool bFoundValidGround = false;

	for (const FHitResult& Hit : OutHits)
	{
		if (IsValid(Hit.GetActor()) && Hit.GetActor()->ActorHasTag(FName("Ground")))
		{
			// 경사각 체크
			const float SlopeDotProduct = FVector::DotProduct(FVector::UpVector, Hit.ImpactNormal);
			const float WalkableSlopeCosine = FMath::Cos(FMath::DegreesToRadians(MaxWalkableSlopeAngle));
			if (SlopeDotProduct < WalkableSlopeCosine)
			{
				// 너무 가파른 땅은 벽으로 간주하고 파괴
				ProcessImpact(Hit, true);
				DestroyProjectile();
				return false;
			}

			BestGroundHit = Hit;
			bFoundValidGround = true;
			break;
		}
	}

	// 유효한 땅을 찾았을 경우 위치/회전 보정
	if (bFoundValidGround)
	{
		const float HoverDistance = SphereRadius * 1.5f;
		const FVector FinalLocation = BestGroundHit.ImpactPoint + BestGroundHit.ImpactNormal * HoverDistance;

		// 앞 방향은 현재 속도 방향을 사용
		FVector ForwardDirection = ProjectileMovementComponent->Velocity.GetSafeNormal();
		if (ForwardDirection.IsNearlyZero())
		{
			ForwardDirection = GetActorForwardVector(); // 멈춰있을 경우를 대비한 예외 처리
		}

		// 윗 방향은 바닥의 경사를 사용
		const FVector UpDirection = BestGroundHit.ImpactNormal;

		// 이 두 방향을 기준으로 회전값을 새로 만듬
		const FRotator FinalRotator = FRotationMatrix::MakeFromXZ(ForwardDirection, UpDirection).Rotator();

		SetActorLocationAndRotation(FinalLocation, FinalRotator, false, nullptr, ETeleportType::TeleportPhysics);

		return true;
	}

	return false;
}

void ADrgProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (OwnerAsc)
		{
			// 주인의 태그 중 Team 카테고리에 속하는 태그를 찾아서 저장.
			OwnerTeamTag = OwnerAsc->GetOwnedGameplayTags().Filter(FGameplayTagContainer(DrgGameplayTags::Team)).
			                         First();
		}
	}

	StartTransform = GetActorTransform();

	// 반복 피해 검사
	if (ProjectileParams.bAllowRepeatDamage)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgProjectile::OnCollisionBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ADrgProjectile::OnCollisionEndOverlap);

		GetWorld()->GetTimerManager().SetTimer(
			PeriodicDamageTimerHandle,
			this,
			&ADrgProjectile::ApplyPeriodicDamage,
			0.05f,
			true
		);
	}
	else
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgProjectile::OnCollisionBeginOverlap);
	}
	// 초기 오버랩 검사
	TArray<AActor*> InitialOverlaps;
	SphereComponent->GetOverlappingActors(InitialOverlaps, AActor::StaticClass());
	for (AActor* OverlappingActor : InitialOverlaps)
	{
		TryProcessTarget(OverlappingActor, FHitResult());
	}

	ProjectileState = EProjectileState::FlyingStraight;

	switch (ProjectileParams.MovementType)
	{
	case EProjectileMovementType::Orbit:
		ProjectileMovementComponent->Deactivate();
		if (UWorld* World = GetWorld())
		{
			if (UProjectileOrbitSubsystem* OrbitSubsystem = World->GetSubsystem<UProjectileOrbitSubsystem>())
			{
				OrbitSubsystem->RegisterOrbitingProjectile(this);
			}
		}
		break;

	case EProjectileMovementType::Arc:
		StartProjectileArc();
		break;

	case EProjectileMovementType::Straight:
	default:
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
		break;
	}

	// 추적 기능이 활성화된 경우, 주기적으로 타겟 탐색 시작
	if (ProjectileParams.bEnableHoming)
	{
		DetectTarget();
		GetWorld()->GetTimerManager().SetTimer(
			DetectTargetTimerHandle, this, &ADrgProjectile::DetectTarget, 0.1f, true);
	}

	if (ProjectileParams.MuzzleVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ProjectileParams.MuzzleVFX,
			StartTransform.GetLocation(),
			StartTransform.GetRotation().Rotator(),
			ProjectileParams.MuzzleScale);
	}

	if (ProjectileParams.MuzzleSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileParams.MuzzleSound, StartTransform.GetLocation());
	}
}

void ADrgProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(PeriodicDamageTimerHandle);
	// 회전 타입일 경우, 서브시스템에서 자신을 등록 해제
	if (ProjectileParams.MovementType == EProjectileMovementType::Orbit)
	{
		if (UWorld* World = GetWorld())
		{
			// 서브시스템이 이미 소멸 중일 수 있으므로 유효성 검사
			if (UProjectileOrbitSubsystem* OrbitSubsystem = World->GetSubsystem<UProjectileOrbitSubsystem>())
			{
				OrbitSubsystem->UnregisterOrbitingProjectile(this);
			}
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ADrgProjectile::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	TryProcessTarget(OtherActor, SweepResult);
}

void ADrgProjectile::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ProjectileParams.bAllowRepeatDamage)
	{
		TargetNextDamageTimeMap.Remove(OtherActor);
	}
}

void ADrgProjectile::ApplyPeriodicDamage()
{
	if (!DamageEffectSpecHandle.IsValid() || TargetNextDamageTimeMap.IsEmpty())
	{
		return;
	}

	UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (!SourceAsc)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	int32 DamageAppliedThisTick = 0;

	// 안전한 순회를 위해 키만 복사
	TArray<AActor*> ActorsToProcess;
	for (const auto& Pair : TargetNextDamageTimeMap)
	{
		ActorsToProcess.Add(Pair.Key);
	}

	// 각 대상에게 지속 피해 적용
	for (AActor* TargetActor : ActorsToProcess)
	{
		if (float* NextDamageTime = TargetNextDamageTimeMap.Find(TargetActor))
		{
			// 순간이동 등으로 벗어났는지 범위 체크
			const float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
			const float OverlapRadius = SphereComponent->GetScaledSphereRadius();

			TArray<AActor*> CurrentOverlappingActors;
			SphereComponent->GetOverlappingActors(CurrentOverlappingActors);
			if (!CurrentOverlappingActors.Contains(TargetActor))
			{
				// 범위를 벗어났으면 제거
				TargetNextDamageTimeMap.Remove(TargetActor);
				continue;
			}

			// 쿨타임 체크
			if (IsValid(TargetActor) && CurrentTime >= *NextDamageTime)
			{
				UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
					TargetActor);
				if (TargetAsc)
				{
					// 피해 적용
					SourceAsc->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetAsc);

					// 이펙트 재생
					FHitResult HitResult;
					HitResult.ImpactPoint = TargetActor->GetActorLocation();
					HitResult.ImpactNormal = (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
					ProcessImpact(HitResult, true);

					// 다음 피해 시간 갱신
					*NextDamageTime = CurrentTime + ProjectileParams.DamageCooldown;
					DamageAppliedThisTick++;
				}
			}
		}
	}

	// 모든 피해 적용 후 관통 체크
	if (!ProjectileParams.bInfinitePierce && DamageAppliedThisTick > 0)
	{
		ProjectileParams.MaxTargetHits -= DamageAppliedThisTick;
		if (ProjectileParams.MaxTargetHits <= 0)
		{
			DestroyProjectile();
		}
	}
}

void ADrgProjectile::StartProjectileArc()
{
	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;

	FVector LaunchVelocity;
	FVector Start = StartTransform.GetLocation();
	FVector Target = StartTransform.TransformPosition(ProjectileParams.TargetOffset);

	const bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		LaunchVelocity,
		Start,
		Target,
		GetWorld()->GetGravityZ(),
		ProjectileParams.ArcHeightRatio
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s에서 %s로 가는 포물선 속도 계산에 실패했습니다."), *Start.ToString(),
		       *Target.ToString());
	}

	if (bSuccess && ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = LaunchVelocity;
	}
}

void ADrgProjectile::DetectTarget()
{
	// 내가 파괴 중이거나, 이미 유도 상태이거나, 투사체를 발사한 주인(Owner)이 사라졌다면 더 이상 탐색할 필요가 없으므로 함수를 종료
	if (!IsValid(this) || ProjectileState == EProjectileState::Homing || !GetOwner())
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
		return;
	}

	// SphereOverlap으로 찾은 액터들을 담을 배열
	TArray<AActor*> DetectedActors;
	// SphereOverlap에서 무시할 액터 목록
	TArray<AActor*> IgnoreActors;
	// 발사한 주인과 투사체 자신은 탐색 대상에서 제외
	IgnoreActors.Add(GetOwner());
	IgnoreActors.Add(this);

	// 탐색할 오브젝트 타입을 Pawn으로 한정
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 현재 투사체 위치를 중심으로, 지정된 반경 내의 액터들을 탐색
	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		ProjectileParams.DetectionRadius,
		ObjectTypes,
		ADrgBaseCharacter::StaticClass(),
		IgnoreActors,
		DetectedActors
	);

#if ENABLE_DRAW_DEBUG
	DrgDebug::DrawDebugSphereWithCVarCheck(
		GetWorld(),
		CVarDrgDebugOverlapQueries, // 어떤 CVar를 쓸지 지정
		GetActorLocation(),
		ProjectileParams.DetectionRadius,
		24,
		bHasOverlap ? FColor::Green : FColor::Red
	);
#endif

	// 주변에 탐색된 액터가 없으면 함수를 종료
	if (DetectedActors.Num() == 0)
	{
		return;
	}

	// 가장 가까운 적
	AActor* ClosestTarget = nullptr;
	// 최대 탐지 거리
	float ClosestTargetDistance = ProjectileParams.DetectionRadius;

	// 탐색된 모든 액터들을 순회해서 가장 가까운 적을 찾기
	for (AActor* TargetCandidate : DetectedActors)
	{
		if (!IsValid(TargetCandidate)) continue;

		// 주인과 후보가 아군이라면 continue
		UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCandidate);
		if (UDrgGameplayStatics::AreTeamsFriendly(OwnerTeamTag, TargetAsc))
		{
			continue;
		}

		// 후보가 죽었다면 continue
		ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(TargetCandidate);
		if (IsValid(TargetCharacter) && TargetCharacter->IsDead())
		{
			continue;
		}

		// 투사체와 후보 사이의 거리를 계산
		const float Distance = FVector::Dist(GetActorLocation(), TargetCandidate->GetActorLocation());
		if (Distance < ClosestTargetDistance)
		{
			ClosestTargetDistance = Distance;
			ClosestTarget = TargetCandidate;
		}
	}

	// 유효한 근접 타겟을 찾았다면 유도 상태로 전환
	if (IsValid(ClosestTarget))
	{
		HomingTarget = ClosestTarget;
		ProjectileState = EProjectileState::Homing;

		// ProjectileMovementComponent을 유도 모드로 설정
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
		ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileParams.HomingAcceleration;

		//타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
	}
}

void ADrgProjectile::DestroyProjectile()
{
	if (bIsDestroy) return;
	bIsDestroy = true;
	GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodicDamageTimerHandle);

	ProjectileMovementComponent->StopMovementImmediately();
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TargetNextDamageTimeMap.Empty();
	DamagedTargetsForSingleHit.Empty();

	if (PointLightComponent) PointLightComponent->SetVisibility(false);
	if (TrailComponent) TrailComponent->Deactivate();
	SetLifeSpan(0.2f);
}

void ADrgProjectile::TryProcessTarget(AActor* TargetActor, const FHitResult& SweepResult)
{
	if (!IsValid(TargetActor) || TargetActor == this || TargetActor == GetOwner())
	{
		return;
	}

	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetAsc)
	{
		ProcessImpact(SweepResult, true);
		DestroyProjectile();
		return;
	}

	if (UDrgGameplayStatics::AreTeamsFriendly(OwnerTeamTag, TargetAsc)) return;

	ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(TargetActor);
	if (TargetCharacter && TargetCharacter->IsDead()) return;

	// 투사체 종류에 따라
	if (ProjectileParams.bAllowRepeatDamage)
	{
		// 반복 피해 처리
		if (!TargetNextDamageTimeMap.Contains(TargetActor))
		{
			// 첫 피해는 즉시 적용
			UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			if (SourceAsc && DamageEffectSpecHandle.IsValid())
			{
				SourceAsc->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetAsc);
			}

			// 다음 피해 시간을 설정
			const float NextDamageTime = GetWorld()->GetTimeSeconds() + ProjectileParams.DamageCooldown;
			TargetNextDamageTimeMap.Add(TargetActor, NextDamageTime);

			// 첫 피해 시에도 관통 처리 및 이펙트 재생이 필요할 수 있음
			ProcessImpact(SweepResult, true);
		}
	}
	else
	{
		// 단일 피해 처리
		if (DamagedTargetsForSingleHit.Contains(TargetActor))
		{
			return;
		}
		DamagedTargetsForSingleHit.Add(TargetActor);

		UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (SourceAsc && DamageEffectSpecHandle.IsValid())
		{
			SourceAsc->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetAsc);
		}

		ProcessImpact(SweepResult, true);

		if (!ProjectileParams.bInfinitePierce)
		{
			ProjectileParams.MaxTargetHits--;
			if (ProjectileParams.MaxTargetHits <= 0)
			{
				DestroyProjectile();
			}
		}
	}
}

void ADrgProjectile::ProcessImpact(const FHitResult& HitResult, bool bFromSweep)
{
	if (bIsDestroy)
	{
		return;
	}
	FVector ImpactLocation;
	FRotator ImpactRotation;
	FVector SurfaceNormal;

	if (bFromSweep && !HitResult.ImpactNormal.IsNearlyZero())
	{
		ImpactLocation = HitResult.ImpactPoint;
		SurfaceNormal = HitResult.ImpactNormal;
	}
	else
	{
		ImpactLocation = GetActorLocation();
		SurfaceNormal = GetActorForwardVector();
	}

	const FVector FinalSpawnLocation = ImpactLocation + SurfaceNormal * ProjectileParams.ImpactOffset;

	switch (ProjectileParams.RotationMethod)
	{
	case EImpactRotationMethod::AlignToProjectile:
		ImpactRotation = GetActorRotation();
		break;
	case EImpactRotationMethod::ZeroRotation:
		ImpactRotation = FRotator::ZeroRotator;
		break;
	case EImpactRotationMethod::AlignToImpactNormal:
	default:
		ImpactRotation = SurfaceNormal.Rotation();
		break;
	}

	// 폭발탄이라면, 범위 피해 적용
	if (ProjectileParams.bEnableAoeOnImpact)
	{
		ExecuteAoeDamage(FinalSpawnLocation);
	}

	// 충돌 이펙트 재생
	PlayImpactEffects(FinalSpawnLocation, ImpactRotation);
}

void ADrgProjectile::PlayImpactEffects(const FVector& Location, const FRotator& Rotation)
{
	if (ProjectileParams.ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileParams.ImpactVFX, Location, Rotation,
		                                               ProjectileParams.ImpactScale);
	}
	if (ProjectileParams.ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileParams.ImpactSound, Location);
	}
}
