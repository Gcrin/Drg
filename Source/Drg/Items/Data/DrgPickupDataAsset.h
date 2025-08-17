// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgPickupDataAsset.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class DRG_API UDrgPickupDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh")
	TSoftObjectPtr<UStaticMesh> DisplayMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh")
	TArray<TSoftObjectPtr<UMaterialInterface>> MaterialOverrides;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform", meta = (MakeEditWidget = true))
	FTransform LocalTransform = FTransform::Identity;

	// 이 아이템을 획득했을 때 플레이어에게 적용될 게임플레이 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Effect")
	TSubclassOf<UGameplayEffect> PickupEffectClass;

#if WITH_EDITOR
	// 에디터에서 프로퍼티가 변경되었을 때 호출되는 함수
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
