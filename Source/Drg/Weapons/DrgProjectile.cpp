// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/System/DrgGameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
