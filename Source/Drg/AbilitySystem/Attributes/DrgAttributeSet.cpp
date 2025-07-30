// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/Player/Data/DrgExperienceData.h"

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
		if (ADrgPlayerCharacter* DrgPlayer = Cast<ADrgPlayerCharacter>(GetOwningActor()))
		{
			float CurrentExperience = GetExperience();
			while (CurrentExperience >= GetMaxExperience() && GetMaxExperience() > 0.f)
			{
				CurrentExperience -= GetMaxExperience();
				SetCharacterLevel(GetCharacterLevel() + 1.f);
				SetMaxExperience(GetMaxExperienceForLevel(DrgPlayer->GetDataTable(), GetCharacterLevel()));
				OnLevelUp.Broadcast(GetOwningActor());
				UE_LOG(LogTemp, Warning, TEXT("Level Up!! / Lv: %.0f(%.1f/%.1f)"), GetCharacterLevel(),
				       CurrentExperience, GetMaxExperience());
			}
			SetExperience(CurrentExperience);
		}
	}
}

float UDrgAttributeSet::GetMaxExperienceForLevel(const UDataTable* DataTable, float CurrentLevel)
{
	check(DataTable != nullptr)

	const FName RowName = FName(*FString::FromInt(CurrentLevel));
	const FString ContextString(TEXT("GetExperienceForLevel"));

	FDrgExperienceData* RowData = DataTable->FindRow<FDrgExperienceData>(RowName, ContextString);

	if (RowData != nullptr)
	{
		return RowData->MaxExperience;
	}

	UE_LOG(LogTemp, Warning, TEXT("DataTable Lv: %.0f의 행을 찾을 수 없습니다."), CurrentLevel);
	return 100.0f;
}
