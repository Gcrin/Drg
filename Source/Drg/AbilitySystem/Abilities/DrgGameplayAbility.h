// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DrgGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DRG_API UDrgGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDrgGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           OUT FGameplayTagContainer* OptionalRelevantTags) const override;


	// 자동 시전 관련 체크 해주고 어빌리티 종료해주는 함수 
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void EndTaskAutoCheck();

	// 자동시전 활성화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability")
	bool bIsOnAutoCast = false;

	//기본 쿨타임(최종 쿨타임 = (기본쿨타임/공격속도)로 적용됨)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Cooldown",
		meta = (UIMin = "0.01", ClampMin = "0.01"))
	float BaseCooldown = 1.0f;

	// 쿨타임 GE에 BaseCooldown 값을 전달할 때 사용할 데이터 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Cooldown")
	FGameplayTag CooldownMagnitudeTag;

	// 고유 쿨타임 태그 이 태그가 적용되어 있을 경우 쿨타임 상태라는 뜻
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability|Cooldown")
	FGameplayTagContainer CooldownTags;

protected:
	void StartCooldown();

	void EndAbilityAutoCheck();

	UFUNCTION()
	void OnCooldownEnded();

	// Task 사용 완료 (블루프린트)
	bool bIsTaskFinished = false;
};
