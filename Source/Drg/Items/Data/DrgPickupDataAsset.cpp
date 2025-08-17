// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPickupDataAsset.h"

#if WITH_EDITOR
void UDrgPickupDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 변경된 프로퍼티의 이름을 가져온다
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		                           ? PropertyChangedEvent.Property->GetFName()
		                           : NAME_None;

	// 변경된 프로퍼티가 'DisplayMesh'일 경우에만 아래 로직을 실행한다
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDrgPickupDataAsset, DisplayMesh))
	{
		// 현재 할당된 메시를 가져온다
		UStaticMesh* CurrentMesh = DisplayMesh.LoadSynchronous();
		if (CurrentMesh)
		{
			//  기존 머티리얼 오버라이드 배열을 깨끗하게 비운다
			MaterialOverrides.Empty();

			//  메시가 사용하는 원본 머티리얼 목록을 가져온다
			const TArray<FStaticMaterial>& StaticMaterials = CurrentMesh->GetStaticMaterials();

			//  원본 머티리얼 목록을 순회하며 오버라이드 배열을 채운다
			for (const FStaticMaterial& Material : StaticMaterials)
			{
				MaterialOverrides.Add(Material.MaterialInterface);
			}
		}
		else
		{
			// 메시가 None으로 설정되면 배열을 비운다
			MaterialOverrides.Empty();
		}
	}
}
#endif
