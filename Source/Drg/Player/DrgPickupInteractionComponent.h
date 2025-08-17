// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrgPickupInteractionComponent.generated.h"


class AInstancedPickupManager;
class UDrgPickupDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DRG_API UDrgPickupInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDrgPickupInteractionComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Drg|Pickup")
	void SetPickupRadius(float NewRadius);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup", meta = (UIMin = "1.0", UIMax = "1000.0"))
	float CollectionRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup", meta = (UIMin = "0.1", UIMax = "10000.0"))
	float PickupRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup", meta = (UIMin = "1.0", UIMax = "10000.0"))
	float PullSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup", meta = (UIMin = "0.0", UIMax = "1.0"))
	float TickInterval = 0.0f;

	// 자석 효과 가속도 (가까울수록 얼마나 빨라질지)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup")
	float MagnetAcceleration = 2.0f;

private:
	void ProcessNearbyPickups(float DeltaTime);
	void ProcessPickupManager(AInstancedPickupManager* Manager, const FVector& OwnerLocation, float DeltaTime);
	void ApplyPickupEffect(UDrgPickupDataAsset* PickupData);
};
