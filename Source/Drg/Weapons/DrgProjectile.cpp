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

	// 오버랩 이벤트가 발생하면 OnSphereOverlap 함수를 호출하도록 바인딩
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgProjectile::OnSphereOverlap);

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

void ADrgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	// 유효성 검사

	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (ProjectileParams.bAllowRepeatDamage)
	{
		// --- 반복 피해 로직 ---
		if (const float* NextDamageTime = DamagedTargetsForRepeatableHit.Find(OtherActor))
		{
			// 쿨타임 중인지 확인
			if (GetWorld()->GetTimeSeconds() < *NextDamageTime)
			{
				return;
			}
		}
	}
	else
	{
		// --- 단일 피해 로직 ---
		if (DamagedTargetsForSingleHit.Contains(OtherActor))
		{
			return;
		}
	}

	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	// 지형지물 또는 ASC가 없는 액터와 충돌한 경우
	if (!TargetAsc)
	{
		ProcessImpact(SweepResult, bFromSweep);
		DestroyProjectile();
		return;
	}

	// 팀 태그가 같을 경우
	if (UDrgGameplayStatics::AreTeamsFriendly(OwnerTeamTag, TargetAsc))
	{
		return;
	}

	// 캐릭터가 죽음 상태일 경우
	ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(OtherActor);
	if (TargetCharacter && TargetCharacter->IsDead()) return;

	// --- 유효한 적 대상에 대한 로직 ---

	// 중복 피해 목록에 추가(반복 피해 구분)
	if (ProjectileParams.bAllowRepeatDamage)
	{
		DamagedTargetsForRepeatableHit.Add(OtherActor, GetWorld()->GetTimeSeconds() + ProjectileParams.DamageCooldown);
	}
	else
	{
		DamagedTargetsForSingleHit.Add(OtherActor);
	}

	ProcessImpact(SweepResult, bFromSweep);

	// 단일 피해 적용 여부
	// 일반탄이거나, 또는 폭발탄이면서 '직격 시 추가 피해' 옵션이 켜진 경우
	const bool bShouldApplyDirectDamage = !ProjectileParams.bEnableAoeOnImpact ||
		ProjectileParams.bApplyBaseDamageToInitialTarget;

	UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

	if (SourceAsc && bShouldApplyDirectDamage && DamageEffectSpecHandle.IsValid())
	{
		SourceAsc->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetAsc);
	}

	// 관통 및 파괴 처리
	if (!ProjectileParams.bInfinitePierce)
	{
		ProjectileParams.MaxTargetHits--;
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
	if (bIsDestroy)
	{
		return;
	}
	bIsDestroy = true;
	GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);

	ProjectileMovementComponent->StopMovementImmediately();
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (PointLightComponent) PointLightComponent->SetVisibility(false);
	if (TrailComponent) TrailComponent->Deactivate();
	SetLifeSpan(0.2f);
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
