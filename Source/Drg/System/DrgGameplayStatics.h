// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DrgGameplayStatics.generated.h"

struct FGameplayTag;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DRG_API UDrgGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 두 AbilitySystemComponent가 같은 팀에 속하는지 확인.
	 * @details "Team" 태그를 기준으로 각 ASC의 팀 태그를 비교.
	 * @param ASC1 비교할 첫 번째 AbilitySystemComponent
	 * @param ASC2 비교할 두 번째 AbilitySystemComponent
	 * @return 같은 팀이면 true, 아니면 false를 반환.
	 */
	UFUNCTION(BlueprintPure, Category = "Drg|AbilitySystem")
	static bool AreTeamsFriendly(UAbilitySystemComponent* ASC1, UAbilitySystemComponent* ASC2);
	/**
	 * @brief GameplayTag와 AbilitySystemComponent가 같은 팀에 속하는지 확인.
	 * @details ASC2에서 "Team" 태그를 찾아 TeamTag1과 일치하는지 비교. 주인이 없는 투사체 등에서 사용하기 위한 편의 함수.
	 * @param TeamTag1 비교할 첫 번째 팀 태그.
	 * @param ASC2 비교할 두 번째 AbilitySystemComponent.
	 * @return 같은 팀이면 true, 아니면 false를 반환.
	 */
	static bool AreTeamsFriendly(const FGameplayTag& TeamTag1, UAbilitySystemComponent* ASC2);
};
