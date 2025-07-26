// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DrgCharacterData.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class DRG_API UDrgCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Drg|Validation")
	bool IsValidData() const;

	// 캐릭터의 모든 스탯 정보가 담긴 데이터 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TObjectPtr<UDataTable> CharacterStatsTable;

	// 캐릭터의 스탯을 초기화하는 데 사용할 게임플레이 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TSubclassOf<UGameplayEffect> StatsInitializerEffect;

	// CharacterStatsTable에서 이 캐릭터의 스탯을 찾기 위한 행(Row) 이름을 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	FName CharacterStatsID;

	// 캐릭터가 시작 시점에 보유할 기본 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	// 캐릭터의 외형을 결정하는 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 캐릭터가 사용할 애니메이션 블루프린트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Animation")
	TSubclassOf<UAnimInstance> AnimClass;
};
