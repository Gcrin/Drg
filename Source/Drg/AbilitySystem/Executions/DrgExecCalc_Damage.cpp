// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"

/**
 * @brief 데미지 계산에 필요한 Attribute를 정의하는 구조체
 * 
 * DECLARE / DEFINE 매크로를 통해 GAS에서 캡처할 Attribute 정보를 정의한다.
 */
struct FDamageStatics
{
	// 공격력 (AttackDamage) 어트리뷰트 정의
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage);
	// 방어력 (Defense) 어트리뷰트 정의
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);

	FDamageStatics()
	{
		// UDrgAttributeSet에서 Source의 AttackDamage를 Snapshot 모드로 캡처
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDrgAttributeSet, AttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDrgAttributeSet, Defense, Target, true);
	}
};

/**
 * @brief 전역에서 접근 가능한 데미지 스태틱 구조체 반환 함수
 * 
 * 싱글톤 방식으로 FDamageStatics 인스턴스를 관리한다.
 * 
 * @return const FDamageStatics& - 정적으로 생성된 Attribute 캡처 정의
 */
static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

UDrgExecCalc_Damage::UDrgExecCalc_Damage()
{
	// 데미지 계산 시 사용할 Attribute를 시스템에 등록
	RelevantAttributesToCapture.Add(DamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UDrgExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                 FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// 현재 적용 중인 GameplayEffectSpec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 캡처한 어트리뷰트에서 값을 가져옴
	float AttackDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDamageDef,
	                                                           FAggregatorEvaluateParameters(), AttackDamage);
	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef,
	                                                           FAggregatorEvaluateParameters(), Defense);


	const float DamageMultiplier = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Ability.Multiplier")), false, 1.0f);

	// 최종 데미지 계산
	float FinalDamage = (AttackDamage * DamageMultiplier) - Defense;

	FinalDamage = FMath::Max(0.1f, FinalDamage);

	UE_LOG(LogTemp, Warning, TEXT("FinalDamage: %.1f (Attack: %.1f * Multiplier: %.1f - Defense: %.1f)"),
		   FinalDamage, AttackDamage, DamageMultiplier, Defense);

	if (FinalDamage > 0.f)
	{
		// 계산된 최종 데미지를 'Health' 어트리뷰트에 적용하라고 출력
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UDrgAttributeSet::GetHealthAttribute(),
				EGameplayModOp::Additive,
				-FinalDamage));
	}
}
