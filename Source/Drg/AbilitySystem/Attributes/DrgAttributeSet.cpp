// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/Player/Data/DrgExperienceData.h"
#include "Drg/System/DrgGameplayTags.h"

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
		// 체력이 0.0f 이하로 떨어졌는지 확인
		if (GetHealth() <= 0.0f)
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

			if (ensure(ASC))
			{
				// 이미 'State.Dead' 태그가 부여되어 있지 않은 경우에만 이벤트 발생
				if (!ASC->HasMatchingGameplayTag(DrgGameplayTags::State_Dead))
				{
					// GA_Drg_Death 실행
					FGameplayEventData Payload;
					Payload.EventTag = DrgGameplayTags::Event_Death;
					ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
				}
			}
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
