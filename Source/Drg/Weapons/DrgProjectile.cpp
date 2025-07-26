// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ADrgProjectile::ADrgProjectile()
{
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

	// 오버랩 이벤트가 발생하면 OnSphereOverlap 함수를 호출하도록 바인딩
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgProjectile::OnSphereOverlap);
}

void ADrgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	// 발사한 주인과는 충돌하지 않도록 처리
	if (OtherActor == GetOwner())
	{
		return;
	}

	ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(OtherActor);
	if (TargetCharacter && TargetCharacter->IsDead())
	{
		// 만약 죽은 캐릭터라면, 아무것도 하지 않고 함수를 종료
		return;
	}

	// 충돌한 액터의 AbilitySystemComponent를 가져옴
	if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		FGameplayEffectContextHandle EffectContext = TargetAsc->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		TargetAsc->ApplyGameplayEffectToSelf(DamageEffectClass.GetDefaultObject(), 1.f, EffectContext);
	}

	Destroy();
}
