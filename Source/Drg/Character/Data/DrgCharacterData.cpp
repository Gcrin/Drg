// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgCharacterData.h"

#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"

bool UDrgCharacterData::IsValidData() const
{
	if (!SkeletalMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 SkeletalMesh가 없습니다."), *GetName());
		return false;
	}
	if (!AnimClass)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 AnimClass가 없습니다."), *GetName());
		return false;
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

	// 3. 스탯 배열의 각 항목이 유효한지 확인합니다.
	for (const FAttributeInitializationData& Data : InitialAttributes)
	{
		if (!Data.Attribute.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'의 InitialAttributes 배열에 유효하지 않은 Attribute 항목이 있습니다."),
			       *GetName());
			return false;
		}
	}

	// DefaultAbilities 배열을 순회하며 비어있는 항목이 있는지 확인
	for (const TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
	{
		if (!Ability)
		{
			UE_LOG(LogTemp, Warning, TEXT("경고: 데이터 애셋 '%s'의 DefaultAbilities 배열에 비어있는 항목이 있습니다."), *GetName());
		}
	}

	return true;
}
