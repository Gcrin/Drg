// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DrgExecCalc_Experience.generated.h"

/**
 * @brief GAS에서 경험치를 계산하는 ExecutionCalculation 클래스
 *
 * 이 클래스는 GameplayEffect 실행 시 호출되어, (EXP_Actor Collision)
 * 획득한 경험치를 기반으로 경험치와 레벨을 계산하고 Attribute에 적용한다.
 */
UCLASS()
class DRG_API UDrgExecCalc_Experience : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDrgExecCalc_Experience();

	/**
	 * @brief 경험치 계산 및 적용을 수행하는 함수
	 *
	 * GAS에서 Execution이 실행될 때 호출되어,
	 * 캡처된 Attribute와 추가 입력값을 이용해 최종 경험치를 계산하고,
	 * 타겟의 Experience 속성에 적용한다.
	 *
	 * @param ExecutionParams 입력값 (소스/타겟/Spec/캡처된 Attribute 등)
	 * @param OutExecutionOutput 최종 결과 (적용할 Modifier) 저장소
	 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
										FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
