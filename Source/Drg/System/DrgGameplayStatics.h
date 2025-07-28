// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DrgGameplayStatics.generated.h"

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
	 * @param ASC1 첫 번째 ASC
	 * @param ASC2 두 번째 ASC
	 * @return 같은 팀이면 true, 아니면 false를 반환.
	 */
	UFUNCTION(BlueprintPure, Category = "Drg|AbilitySystem")
	static bool AreTeamsFriendly(UAbilitySystemComponent* ASC1, UAbilitySystemComponent* ASC2);
};
