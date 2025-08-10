// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrgPickupBase.generated.h"

class UGameplayEffect;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DRG_API ADrgPickupBase : public AActor
{
	GENERATED_BODY()

public:
	ADrgPickupBase();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Drg|Pickup")
	virtual void ApplyEffect(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Pickup")
	TSubclassOf<UGameplayEffect> PickupEffectClass;

private:
	void AdjustSpawnLocationToGround();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
