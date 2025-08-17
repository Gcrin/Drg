// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPickupInteractionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Drg/Items/InstancedPickupManager.h"
#include "Drg/Items/Data/DrgPickupDataAsset.h"

UDrgPickupInteractionComponent::UDrgPickupInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = TickInterval;
}

void UDrgPickupInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessNearbyPickups(DeltaTime);
}

void UDrgPickupInteractionComponent::SetPickupRadius(float NewRadius)
{
	PickupRadius = NewRadius;
}

void UDrgPickupInteractionComponent::ProcessNearbyPickups(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector OwnerLocation = Owner->GetActorLocation();

	for (TActorIterator<AInstancedPickupManager> Itr(World); Itr; ++Itr)
	{
		AInstancedPickupManager* Manager = *Itr;
		if (Manager && IsValid(Manager))
		{
			ProcessPickupManager(Manager, OwnerLocation, DeltaTime);
		}
	}
}

void UDrgPickupInteractionComponent::ProcessPickupManager(AInstancedPickupManager* Manager,
                                                          const FVector& OwnerLocation, float DeltaTime)
{
	if (!Manager) return;

	const int32 InstanceCount = Manager->GetInstanceCount();
	const float PickupRadiusSq = PickupRadius * PickupRadius;

	for (int32 i = InstanceCount - 1; i >= 0; --i)
	{
		FTransform InstanceTransform;
		if (!Manager->GetInstanceTransform(i, InstanceTransform))
			continue;

		const FVector InstanceLocation = InstanceTransform.GetLocation();
		
		const float DistanceSq = FVector::DistSquared(OwnerLocation, InstanceLocation);
		// 범위 밖이면 바로 스킵
		if (DistanceSq > PickupRadiusSq) continue;
      
		// 범위 내 아이템만 처리 (정확한 거리 계산)
		const float Distance = FMath::Sqrt(DistanceSq);

		if (Distance <= CollectionRadius)
		{
			// 아이템 획득
			Manager->RemoveInstance(i);
			ApplyPickupEffect(Manager->GetPickupData());
		}
		else if (Distance <= PickupRadius)
		{
			// 자석 효과 플레이어 쪽으로 끌어당기기
			const float DistanceRatio = (PickupRadius - Distance) / PickupRadius;
			const float AcceleratedSpeed = PullSpeed * (1.0f + DistanceRatio * MagnetAcceleration);

			const FVector NewLocation = FMath::VInterpConstantTo(
				InstanceLocation,
				OwnerLocation,
				DeltaTime,
				AcceleratedSpeed
			);

			InstanceTransform.SetLocation(NewLocation);
			Manager->UpdateInstanceTransform(i, InstanceTransform);
		}
	}
}

void UDrgPickupInteractionComponent::ApplyPickupEffect(UDrgPickupDataAsset* PickupData)
{
	if (!PickupData || !PickupData->PickupEffectClass) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		ASC->ApplyGameplayEffectToSelf(PickupData->PickupEffectClass.GetDefaultObject(), 1.0f, Context);
	}
}
