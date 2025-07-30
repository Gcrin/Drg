// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPickup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

ADrgPickup::ADrgPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetSphereRadius(80.0f);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADrgPickup::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrgPickup::OnSphereOverlap);
}

void ADrgPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	ApplyEffect(OtherActor);
}

void ADrgPickup::ApplyEffect(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
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

	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	Destroy();
}
