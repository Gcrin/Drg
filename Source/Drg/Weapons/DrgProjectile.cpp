// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/System/DrgGameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
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

	//MeshComponent 설정
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 외형은 충돌 계산 안함

	// 발사체 움직임 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 중력 영향 안받음
}

void ADrgProjectile::BeginPlay()
{
	Super::BeginPlay();

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
	if (ProjectileParams.bEnableChase)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DetectTargetTimerHandle, this, &ADrgProjectile::DetectTarget, 0.1f, true);
	}
}

void ADrgProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckDistance();
}

void ADrgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;

	if (OtherActor == this || OtherActor == GetOwner()) return;

	ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(OtherActor);
	if (TargetCharacter && TargetCharacter->IsDead()) return;

	UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (UDrgGameplayStatics::AreTeamsFriendly(OwnerAsc, TargetAsc))
	{
		return;
	}

	if (TargetAsc && DamageEffectSpecHandle.IsValid())
	{
		if (FGameplayEffectSpec* SpecToApply = DamageEffectSpecHandle.Data.Get())
		{
			UAbilitySystemComponent* SourceAsc = SpecToApply->GetContext().GetInstigatorAbilitySystemComponent();

			if (SourceAsc)
			{
				SourceAsc->ApplyGameplayEffectSpecToTarget(*SpecToApply, TargetAsc);
			}
		}
	}

	DestroyProjectile();
}


void ADrgProjectile::SetMaxRange(float ArgMaxRange)
{
	if (MaxRange > 0.f)
		MaxRange = ArgMaxRange;
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
		ProjectileParams.ArcParam
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to calculate arc velocity from %s to %s"), *Start.ToString(),
		       *Target.ToString());
	}

	if (bSuccess && ProjectileMovement)
	{
		ProjectileMovement->Velocity = LaunchVelocity;
	}
}

void ADrgProjectile::DetectTarget()
{
	// 이미 유도 상태이거나, 투사체를 발사한 주인(Owner)이 사라졌다면 더 이상 탐색할 필요가 없으므로 함수를 종료
	if (ProjectileState == EProjectileState::Homing || !GetOwner())
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
		return;
	}

	// SphereOverlap으로 찾은 액터들을 담을 배열
	TArray<AActor*> OutActors;
	// SphereOverlap에서 무시할 액터 목록
	TArray<AActor*> IgnoreActors;
	// 발사한 주인과 투사체 자신은 탐색 대상에서 제외
	IgnoreActors.Add(GetOwner());
	IgnoreActors.Add(this);

	// 탐색할 오브젝트 타입을 Pawn으로 한정
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 현재 투사체 위치를 중심으로, 지정된 반경 내의 액터들을 탐색
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		ProjectileParams.DetectionRadius,
		ObjectTypes,
		ADrgBaseCharacter::StaticClass(),
		IgnoreActors,
		OutActors
	);

	// 주변에 탐색된 액터가 없으면 함수를 종료
	if (OutActors.Num() == 0)
	{
		return;
	}

	// 가장 가까운 적
	AActor* NearEnemy = nullptr;
	// 최대 탐지 거리
	float NearDistance = ProjectileParams.DetectionRadius;

	// 발사한 주인의 어빌리티 시스템 컴포넌트를 가져와 팀을 식별하는 데 사용
	UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

	// 탐색된 모든 액터들을 순회해서 가장 가까운 적을 찾기
	for (AActor* TargetCandidate : OutActors)
	{
		if (!IsValid(TargetCandidate)) continue;

		// 주인과 후보가 아군이라면 continue
		UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCandidate);
		if (OwnerAsc && TargetAsc && UDrgGameplayStatics::AreTeamsFriendly(OwnerAsc, TargetAsc))
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
		if (Distance < NearDistance)
		{
			NearDistance = Distance;
			NearEnemy = TargetCandidate;
		}
	}

	// 유효한 근접 타겟을 찾았다면 유도 상태로 전환
	if (IsValid(NearEnemy))
	{
		HomingTarget = NearEnemy;
		ProjectileState = EProjectileState::Homing;

		// ProjectileMovementComponent을 유도 모드로 설정
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		ProjectileMovement->HomingAccelerationMagnitude = ProjectileParams.ChaseSpeed;

		//타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
	}
}

void ADrgProjectile::CalcDistance()
{
	if (MaxRange > 0.f)
	{
		FVector CurrentLocation = this->GetActorLocation();
		FVector StartLocation = StartTransform.GetLocation();

		MoveDistance = FVector::Dist(CurrentLocation, StartLocation);
	}
}

void ADrgProjectile::CheckDistance()
{
	CalcDistance();

	if (MoveDistance >= MaxRange && MaxRange > 0.f)
	{
		DestroyProjectile();
	}
}

void ADrgProjectile::DestroyProjectile()
{
	Destroy();
}
