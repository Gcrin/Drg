// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgCharacterData.h"

#include "DrgCharacterStats.h"
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

	// 스탯 데이터 테이블 참조가 유효한지 확인
	if (!CharacterStatsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 CharacterStatsTable이 할당되지 않았습니다."), *GetName());
		return false;
	}

	if (!StatsInitializerEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 StatsInitializerEffect가 없습니다."), *GetName());
		return false;
	}
	
	// 스탯 ID가 유효한지 확인
	if (CharacterStatsID == NAME_None)
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 애셋 '%s'에 CharacterStatsID가 설정되지 않았습니다."), *GetName());
		return false;
	}

	// 실제로 데이터 테이블에 해당 ID를 가진 행이 존재하는지 확인
	if (!CharacterStatsTable->FindRow<FDrgCharacterStats>(CharacterStatsID, TEXT("")))
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 데이터 테이블 '%s'에서 ID '%s'를 찾을 수 없습니다."), *CharacterStatsTable->GetName(), *CharacterStatsID.ToString());
		return false;
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
