// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAttributeSet.h"

#include "GameplayEffectExtension.h"

UDrgAttributeSet::UDrgAttributeSet()
{
}

void UDrgAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UDrgAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 이번에 변경된 Attribute가 Health인지 확인
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// SetHealth를 호출하여 PreAttributeChange의 Clamp 로직이 실행되도록 유도.
		// ExecutionCalculation 도입 후 중복 호출로 제거 나중에 관련 문제 생기면 다시 사용할 것
		//SetHealth(GetHealth());

		// 체력이 0 이하가 되었는지 확인
		if (GetHealth() <= 0.0f)
		{
			// OnDeath 이벤트를 구독하고 있는 모든 대상에게 신호를 보냄.
			OnDeath.Broadcast(GetOwningActor());
		}
	}

	if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		float CurrentExperience = GetExperience();
		while (CurrentExperience >= GetMaxExperience() && GetMaxExperience() > 0.f)
		{
			CurrentExperience -= GetMaxExperience();
			SetCharacterLevel(GetCharacterLevel() + 1.f);
			OnLevelUp.Broadcast(GetOwningActor());
		}
		SetExperience(CurrentExperience);
	}
}
