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
	// 캐릭터의 외형을 결정하는 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 캐릭터가 사용할 애니메이션 블루프린트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Animation")
	TSubclassOf<UAnimInstance> AnimClass;
	
	// 캐릭터의 기본 스탯을 정의하는 게임플레이 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributes;
	// 캐릭터가 시작 시점에 보유할 기본 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
};
