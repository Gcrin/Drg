// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgCharacterData.h"

#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"

bool UDrgCharacterData::IsValidData() const
{
	if (!SkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("경고: 데이터 애셋 '%s'에 SkeletalMesh가 없습니다."), *GetName());
	}

	if (!AnimClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("경고: 데이터 애셋 '%s'에 AnimClass가 없습니다."), *GetName());
	}

	for (const TSoftObjectPtr<UMaterialInterface>& Material : MaterialOverrides)
	{
		if (!Material)
		{
			UE_LOG(LogTemp, Warning, TEXT("경고: 데이터 애셋 '%s'의 DefaultAbilities 배열에 비어있는 항목이 있습니다."), *GetName());
		}
	}

	if (!StatsInitializerEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 StatsInitializerEffect가 없습니다."), *GetName());
		return false;
	}

	if (InitialAttributes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 InitialAttributes가 하나도 없습니다."), *GetName());
		return false;
	}

	for (const FAttributeInitializationData& Data : InitialAttributes)
	{
		if (!Data.Attribute.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'의 InitialAttributes 배열에 유효하지 않은 Attribute 항목이 있습니다."),
			       *GetName());
			return false;
		}
	}

	for (const TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
	{
		if (!Ability)
		{
			UE_LOG(LogTemp, Warning, TEXT("경고: 데이터 애셋 '%s'의 DefaultAbilities 배열에 비어있는 항목이 있습니다."), *GetName());
		}
	}

	return true;
}

#if WITH_EDITOR
void UDrgCharacterData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 변경된 프로퍼티의 이름을 가져온다
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		                           ? PropertyChangedEvent.Property->GetFName()
		                           : NAME_None;

	// 변경된 프로퍼티가 'SkeletalMesh'일 경우에만 아래 로직을 실행한다
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDrgCharacterData, SkeletalMesh))
	{
		// 현재 할당된 스켈레탈 메시를 가져온다
		USkeletalMesh* CurrentMesh = SkeletalMesh.Get();
		if (CurrentMesh)
		{
			//  기존 머티리얼 오버라이드 배열을 깨끗하게 비운다
			MaterialOverrides.Empty();

			//  메시가 사용하는 원본 머티리얼 목록을 가져온다
			const TArray<FSkeletalMaterial>& SkeletalMaterials = CurrentMesh->GetMaterials();

			//  원본 머티리얼 목록을 순회하며 오버라이드 배열을 채운다
			for (const FSkeletalMaterial& Material : SkeletalMaterials)
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
