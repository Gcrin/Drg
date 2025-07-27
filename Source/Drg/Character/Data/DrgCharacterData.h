// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataAsset.h"
#include "DrgCharacterData.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAttributeInitializationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;
};

UCLASS()
class DRG_API UDrgCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Drg|Validation")
	bool IsValidData() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS|Stats")
	TArray<FAttributeInitializationData> InitialAttributes;

	// 캐릭터의 스탯을 초기화하는 데 사용할 게임플레이 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TSubclassOf<UGameplayEffect> StatsInitializerEffect;

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
