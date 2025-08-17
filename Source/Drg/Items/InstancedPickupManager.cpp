// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedPickupManager.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Data/DrgPickupDataAsset.h"

AInstancedPickupManager::AInstancedPickupManager()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	SetRootComponent(InstancedMeshComponent);
	InstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AInstancedPickupManager::Initialize(UDrgPickupDataAsset* Data)
{
	if (!Data)
	{
		check(false);
		return;
	}

	PickupData = Data;

	// 데이터 애셋에서 메시와 머티리얼 정보를 비동기 로드 후 설정할 수 있지만 시간이 없으니까 동기로..
	UStaticMesh* MeshToSet = PickupData->DisplayMesh.LoadSynchronous();
	if (MeshToSet)
	{
		InstancedMeshComponent->SetStaticMesh(MeshToSet);
	}

	for (int32 i = 0; i < PickupData->MaterialOverrides.Num(); ++i)
	{
		UMaterialInterface* MaterialToSet = PickupData->MaterialOverrides[i].LoadSynchronous();
		if (MaterialToSet)
		{
			InstancedMeshComponent->SetMaterial(i, MaterialToSet);
		}
	}
}

void AInstancedPickupManager::AddInstance(const FTransform& NewTransform)
{
	InstancedMeshComponent->AddInstance(NewTransform);
}

void AInstancedPickupManager::RemoveInstance(int32 InstanceIndex)
{
	InstancedMeshComponent->RemoveInstance(InstanceIndex);
}

void AInstancedPickupManager::UpdateInstanceTransform(int32 InstanceIndex, const FTransform& NewTransform)
{
	InstancedMeshComponent->UpdateInstanceTransform(InstanceIndex, NewTransform, true, true, true);
}

int32 AInstancedPickupManager::GetInstanceCount() const
{
	return InstancedMeshComponent->GetInstanceCount();
}

bool AInstancedPickupManager::GetInstanceTransform(int32 InstanceIndex, FTransform& OutTransform) const
{
	return InstancedMeshComponent->GetInstanceTransform(InstanceIndex, OutTransform);
}
