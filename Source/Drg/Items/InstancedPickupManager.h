// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancedPickupManager.generated.h"

class UDrgPickupDataAsset;

UCLASS()
class DRG_API AInstancedPickupManager : public AActor
{
	GENERATED_BODY()

public:
	AInstancedPickupManager();

	// 이 매니저가 어떤 종류의 아이템을 담당할지 초기화
	void Initialize(UDrgPickupDataAsset* Data);

	// 지정된 위치에 새 아이템 인스턴스를 추가
	void AddInstance(const FTransform& NewTransform);

	// 지정된 인덱스의 아이템 인스턴스를 제거
	void RemoveInstance(int32 InstanceIndex);

	// 지정된 인덱스의 아이템 인스턴스 위치를 업데이트 (자석 효과용)
	void UpdateInstanceTransform(int32 InstanceIndex, const FTransform& NewTransform);

	// 이 매니저가 담당하는 아이템의 데이터를 반환
	FORCEINLINE UDrgPickupDataAsset* GetPickupData() const { return PickupData; }

	// 이 매니저가 가진 모든 인스턴스의 개수를 반환
	int32 GetInstanceCount() const;

	// 지정된 인덱스의 인스턴스 트랜스폼을 가져옴
	bool GetInstanceTransform(int32 InstanceIndex, FTransform& OutTransform) const;

protected:
	// 아이템 메시를 실제로 렌더링하는 인스턴스드 스태틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> InstancedMeshComponent;

	// 이 매니저가 담당하는 아이템의 정보를 담은 데이터 애셋
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDrgPickupDataAsset> PickupData;
};
