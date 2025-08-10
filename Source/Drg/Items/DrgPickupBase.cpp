// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPickupBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Drg/System/DrgGameplayTags.h"

ADrgPickupBase::ADrgPickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(80.0f);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADrgPickupBase::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgPickupBase::OnSphereOverlap);

	AdjustSpawnLocationToGround();
}

void ADrgPickupBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	ApplyEffect(OtherActor);
}

void ADrgPickupBase::ApplyEffect(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}


	if (!TargetASC->HasMatchingGameplayTag(DrgGameplayTags::Team_Player))
	{
		return;
	}

	if (!PickupEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: PickupEffectClass가 설정되지 않았습니다."), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
		PickupEffectClass, 1.0f, EffectContext);

	if (EffectSpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}

	Destroy();
}

void ADrgPickupBase::AdjustSpawnLocationToGround()
{
	if (!SphereComponent) return;

	const float SphereRadius = SphereComponent->GetScaledSphereRadius();
	const FVector StartLocation = GetActorLocation() + FVector(0.f, 0.f, SphereRadius + 50.f);
	const FVector EndLocation = GetActorLocation() - FVector(0.f, 0.f, 1000.f);

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility
	);

	if (bHit)
	{
		SetActorLocation(HitResult.Location);
	}
}
