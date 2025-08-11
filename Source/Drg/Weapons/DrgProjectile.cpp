// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/System/DrgDebug.h"
#include "Drg/System/DrgGameplayStatics.h"
#include "Drg/System/DrgGameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADrgProjectile::ADrgProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 충돌체인 SphereComponent 설정
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 폰하고만 오버랩 이벤트 발생
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 벽과도 충돌
	SphereComponent->SetSphereRadius(10.f);

	//MeshComponent 설정
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 외형은 충돌 계산 안함

	// 발사체 움직임 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 중력 영향 안받음
	// 투사체가 속도 방향을 따라 회전
	ProjectileMovement->bRotationFollowsVelocity = true;

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

void ADrgProjectile::SetDamageCooldownForTarget(AActor* TargetActor)
{
	if (!TargetActor || ProjectileParams.OrbitalDamageCooldown <= 0.f) return;

	FTimerHandle& CooldownTimerHandle = RecentlyDamagedActors.FindOrAdd(TargetActor);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnDamageCooldownExpired"), TWeakObjectPtr<AActor>(TargetActor));

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, TimerDelegate, ProjectileParams.OrbitalDamageCooldown, false);
}

void ADrgProjectile::OnDamageCooldownExpired(TWeakObjectPtr<AActor> TargetToRemove)
{
	if (TargetToRemove.IsValid())
	{
		RecentlyDamagedActors.Remove(TargetToRemove.Get());
	}
}

void ADrgProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 바인딩은 모드와 상관없이 필요하므로 가장 먼저 수행합니다.
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgProjectile::OnSphereOverlap);

	// 회전 모드일 경우, 자체 이동 로직을 비활성화하고 즉시 종료합니다.
	if (ProjectileParams.bIsOrbital)
	{
		if (ensure(ProjectileMovement))
		{
			ProjectileMovement->Deactivate();
		}
		// 회전 모드에서는 이후의 발사/유도 로직이 필요 없습니다.
		return;
	}

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

	// 포물선 기능이 활성화되지 않았다면 직선으로 발사
	if (!ProjectileParams.bEnableArc)
	{
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}
	else
	{
		StartProjectileArc();
	}

	// 추적 기능이 활성화된 경우, 주기적으로 타겟 탐색 시작
	if (ProjectileParams.bEnableHoming)
	{
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

void ADrgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	// --- 유효한 적 대상인지 확인 ---

	// if (!IsValid(OtherActor) || DamagedActors.Contains(OtherActor) || OtherActor == this || OtherActor ==
	// 	GetOwner())
	// {
	// 	return;
	// }
	if (!IsValid(OtherActor) || RecentlyDamagedActors.Contains(OtherActor) || OtherActor ==
		GetOwner())
	{
		return;
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

	// 중복 피해 목록에 추가
	// DamagedActors.Add(OtherActor);
	//회전체 추가후
	SetDamageCooldownForTarget(OtherActor);

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
	if (!ProjectileParams.bEnableArc)
		return;
	ProjectileMovement->ProjectileGravityScale = 1.0f;

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

	if (bSuccess && ProjectileMovement)
	{
		ProjectileMovement->Velocity = LaunchVelocity;
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
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		ProjectileMovement->HomingAccelerationMagnitude = ProjectileParams.HomingAcceleration;

		//타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
	}
}

void ADrgProjectile::DestroyProjectile()
{
	GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);

	ProjectileMovement->StopMovementImmediately();
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (MeshComponent) MeshComponent->SetVisibility(false);
	if (PointLightComponent) PointLightComponent->SetVisibility(false);

	SetLifeSpan(0.2f);
}

void ADrgProjectile::ProcessImpact(const FHitResult& HitResult, bool bFromSweep)
{
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
