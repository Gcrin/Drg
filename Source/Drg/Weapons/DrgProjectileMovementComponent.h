// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrgProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class DRG_API UDrgProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
