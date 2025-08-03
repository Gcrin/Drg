// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgMMC_AttackSpeedCooldown.h"

#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"

UDrgMMC_AttackSpeedCooldown::UDrgMMC_AttackSpeedCooldown()
{
	// 캡처 정의를 담을 구조체 변수를 선언
	FGameplayEffectAttributeCaptureDefinition AttackSpeedDef;
	// 어떤 어트리뷰트를 캡처할지 설정
	AttackSpeedDef.AttributeToCapture = UDrgAttributeSet::GetAttackSpeedAttribute();
	// 어트리뷰트를 누구에게서 가져올지 설정 (Target = 이펙트가 적용되는 대상)
	AttackSpeedDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// 스냅샷(Snapshot) 여부를 설정합니다. (false = 실시간 값, true = 이펙트 적용 시점의 값)
	AttackSpeedDef.bSnapshot = false;

	// 완성된 캡처 정의를 '캡처할 어트리뷰트 목록'에 추가
	RelevantAttributesToCapture.Add(AttackSpeedDef);
}

float UDrgMMC_AttackSpeedCooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// SetByCaller로 전달된 BaseCooldown 값을 가져옴
	const float BaseCooldown = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown.Duration")), false, 1.0f);

	FAggregatorEvaluateParameters EvaluationParameters;
	// 캡처된 AttackSpeed 값을 가져옴
	float AttackSpeed = 0.f;
	GetCapturedAttributeMagnitude(RelevantAttributesToCapture[0], Spec, EvaluationParameters, AttackSpeed);

	if (AttackSpeed <= 0.f)
	{
		AttackSpeed = 1.f;
	}

	// 최종 쿨타임을 계산하여 반환
	return BaseCooldown / AttackSpeed;
}
