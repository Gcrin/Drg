// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgMagnetManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Drg/Items/DrgPickupBase.h"

ADrgMagnetManager::ADrgMagnetManager()
{
	PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.TickInterval = 0.1f;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}

void ADrgMagnetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MagnetRadius <= 0.0f) return;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> OverlappedActors;

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		MagnetRadius,
		ObjectTypes,
		ADrgPickupBase::StaticClass(),
		TArray<AActor*>(),
		OverlappedActors
	);

	for (AActor* OverlappedActor : OverlappedActors)
	{
		// VInterpTO 멀수록 빠르게
		const FVector NewLocation = FMath::VInterpConstantTo(
			OverlappedActor->GetActorLocation(),
			GetActorLocation(),
			DeltaTime,
			PullSpeed
		);
		OverlappedActor->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ADrgMagnetManager::SetMagnetRadius(float NewRadius)
{
	MagnetRadius = NewRadius;
}
