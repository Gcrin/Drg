// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS|Tags")
	FGameplayTagContainer InitialTags;

	// 캐릭터 GA_Drg_Death 트리거 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS|Tags")
	FGameplayTag DeathTypeTag;

	// 캐릭터의 스탯을 초기화하는 데 사용할 어트리뷰트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS|Stats")
	TArray<FAttributeInitializationData> InitialAttributes;

	// 캐릭터의 스탯을 초기화하는 데 사용할 게임플레이 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TSubclassOf<UGameplayEffect> StatsInitializerEffect;

	// 캐릭터가 시작 시점에 보유할 기본 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	// 캐릭터가 시작 시점에 보유할 기본 이펙트 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	// 캐릭터가 사용할 어빌리티별 애님 몽타주 목록 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Animation")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AbilityMontages;

	// 태그에 해당하는 몽타주를 찾아 반환하는 헬퍼 함수 
	UFUNCTION(BlueprintCallable, Category = "Drg|Animation")
	UAnimMontage* GetMontageForAbilityTag(const FGameplayTag& AbilityTag) const;

	// 캐릭터의 외형을 결정하는 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh")
	TArray<TSoftObjectPtr<UMaterialInterface>> MaterialOverrides;

	// 스켈레탈 메시 스케일 최소값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh|Scale")
	float MeshScaleMin = 1.0f;

	// 스켈레탈 메시 스케일 최대값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh|Scale")
	float MeshScaleMax = 1.0f;

	// 캡슐 컴포넌트 HalfHeight(메시 스케일 1.0 기준)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh|Scale")
	float CapsuleHalfHeight = 88.0f;

	// 캡슐 컴포넌트 Radius(메시 스케일 1.0 기준, 캡슐 컴포넌트 HalfHeight보다 작거나 같아야함)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Mesh|Scale")
	float CapsuleRadius = 34.0f;

	// 캐릭터가 사용할 애니메이션 블루프린트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Animation")
	TSubclassOf<UAnimInstance> AnimClass;

	// AI 캐릭터가 사용할 비해비어 트리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// 이 캐릭터가 죽었을 때 사용할 드롭 규칙을 식별하는 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Drop")
	FGameplayTag DropTag;

#if WITH_EDITOR
	// 에디터에서 프로퍼티가 변경되었을 때 호출되는 함수
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
