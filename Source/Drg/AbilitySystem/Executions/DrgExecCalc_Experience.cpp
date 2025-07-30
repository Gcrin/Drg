// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgExecCalc_Experience.h"

#include "AbilitySystemComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"

/**
 * @brief 경험치 계산에 필요한 Attribute를 정의하는 구조체
 *
 * DECLARE / DEFINE 매크로를 통해 GAS에서 캡처할 Attribute 정보를 정의한다.
 */
struct FExperienceStatics
{
	// 경험치 (Experience) 어트리뷰트 정의
	DECLARE_ATTRIBUTE_CAPTUREDEF(Experience);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxExperience);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CharacterLevel);

	FExperienceStatics()
	{
		// UDrgAttributeSet에서 Target의 Experience를 Snapshot 모드로 캡처
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDrgAttributeSet, Experience, Target, true);
		// UDrgAttributeSet에서 Target의 MaxExperience를 Snapshot 모드로 캡처
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDrgAttributeSet, MaxExperience, Target, true);
		// UDrgAttributeSet에서 Target의 CharacterLevel를 Snapshot 모드로 캡처
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDrgAttributeSet, CharacterLevel, Target, true);
	}
};

/**
 * @brief 전역에서 접근 가능한 경험치 스태틱 구조체 반환 함수
 * 싱글톤 방식으로 FExperienceStatics 인스턴스를 관리한다.
 * @return const FExperienceStatics& - 정적으로 생성된 Attribute 캡처 정의
 */
static const FExperienceStatics& ExperienceStatics()
{
	static FExperienceStatics ExpStatics;
	return ExpStatics;
}

UDrgExecCalc_Experience::UDrgExecCalc_Experience()
{
	// 경험치 계산 시 사용할 Attribute를 시스템에 등록
	RelevantAttributesToCapture.Add(ExperienceStatics().ExperienceDef);
	RelevantAttributesToCapture.Add(ExperienceStatics().MaxExperienceDef);
	RelevantAttributesToCapture.Add(ExperienceStatics().CharacterLevelDef);
}

void UDrgExecCalc_Experience::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
													 FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// 현재 적용 중인 GameplayEffectSpec
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 캡처한 어트리뷰트에서 값을 가져옴
	float Experience = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		ExperienceStatics().ExperienceDef, FAggregatorEvaluateParameters(), Experience);
	float MaxExperience = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		ExperienceStatics().MaxExperienceDef, FAggregatorEvaluateParameters(), MaxExperience);
	float CharacterLevel = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		ExperienceStatics().CharacterLevelDef, FAggregatorEvaluateParameters(), CharacterLevel);

	// 경험치 테이블 미구현 임시 작성
	const float ExperienceAmount = 50.f;

	float NewExperience = Experience + ExperienceAmount;
	bool bIsLevelUp = false;

	UE_LOG(LogTemp, Warning, TEXT("%.1f Exp Gain"), ExperienceAmount);
	
	while (NewExperience > MaxExperience && MaxExperience > 0)
	{
		NewExperience -= MaxExperience;
		CharacterLevel += 1.f;
		bIsLevelUp = true;

		UE_LOG(LogTemp, Warning, TEXT("Level Up!! / Lv: %.0f (%.1f/%.1f)"),
			CharacterLevel, Experience, MaxExperience);
		// MaxExperience 설정 로직 추가 필요
		// OnLevelUp 구현 필요 -> 업그레이드, 레벨업 효과들
	}

	// 최종 값을 어트리뷰트에 적용하라고 출력 
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UDrgAttributeSet::GetExperienceAttribute(),
			EGameplayModOp::Override,
			NewExperience));

	if (bIsLevelUp)
	{
		OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UDrgAttributeSet::GetCharacterLevelAttribute(),
			EGameplayModOp::Override,
			CharacterLevel));

		OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UDrgAttributeSet::GetMaxExperienceAttribute(),
			EGameplayModOp::Override,
			MaxExperience));
	}
}
