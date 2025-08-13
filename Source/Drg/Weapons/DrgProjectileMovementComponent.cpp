// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgProjectileMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Drg/System/DrgGameplayTags.h"

void UDrgProjectileMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsHomingProjectile && HomingTargetComponent.IsValid())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			HomingTargetComponent->GetOwner());

		if (TargetASC && TargetASC->HasMatchingGameplayTag(DrgGameplayTags::State_Dead))
		{
			HomingTargetComponent = nullptr;
		}
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
