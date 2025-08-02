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
	PrimaryActorTick.bCanEverTick = false;

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

	// 투사체가 속도 방향을 따라 회전
	ProjectileMovement->bRotationFollowsVelocity = true;
}

void ADrgProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (OwnerAsc)
		{
			const FGameplayTag TeamRootTag = FGameplayTag::RequestGameplayTag(TEXT("Team"));
			// 주인의 태그 중 Team 카테고리에 속하는 태그를 찾아서 저장.
			OwnerTeamTag = OwnerAsc->GetOwnedGameplayTags().Filter(FGameplayTagContainer(TeamRootTag)).First();
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
}

void ADrgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;
	if (DamagedActors.Contains(OtherActor)) return;
	if (OtherActor == this || OtherActor == GetOwner()) return;

	ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(OtherActor);
	if (TargetCharacter && TargetCharacter->IsDead()) return;

	UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (UDrgGameplayStatics::AreTeamsFriendly(OwnerTeamTag, TargetAsc))
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

				// 중복 피해 방지 목록에 추가합니다.
				DamagedActors.Add(OtherActor);
				if (!ProjectileParams.bInfinitePierce)
				{
					ProjectileParams.MaxTargetHits--;
					if (ProjectileParams.MaxTargetHits <= 0)
					{
						DestroyProjectile();
						return;
					}
				}
			}
		}
	}
	else
	{
		DestroyProjectile();
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

	// 탐색된 모든 액터들을 순회해서 가장 가까운 적을 찾기
	for (AActor* TargetCandidate : OutActors)
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
		ProjectileMovement->HomingAccelerationMagnitude = ProjectileParams.HomingAcceleration;

		//타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DetectTargetTimerHandle);
	}
}

void ADrgProjectile::DestroyProjectile()
{
	Destroy();
}
