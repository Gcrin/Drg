// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/GameplayMessageSubsystem.h"
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
					const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
					FGameplayEventData Payload;
					Payload.EventTag = DrgGameplayTags::Event_Death;
					Payload.Instigator = Context.GetOriginalInstigator(); // 가해자 정보
					Payload.Target = GetOwningActor(); // 피해자 정보 (나 자신)
					Payload.EventMagnitude = Data.EvaluatedData.Magnitude; // 받은 대미지량
					Payload.ContextHandle = Context; // 모든 컨텍스트 정보를 통째로 전달
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

void UDrgAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (GetOwningActor() && GetOwningActor()->IsA<ADrgPlayerCharacter>())
	{
		if (OldValue == NewValue) return;
	
		if (Attribute == GetHealthAttribute())
		{
			BroadcastAttributeChange(EAttributeType::Health, NewValue);
		}
		else if (Attribute == GetMaxHealthAttribute())
		{
			BroadcastAttributeChange(EAttributeType::MaxHealth, NewValue);
		}
		else if (Attribute == GetExperienceAttribute())
		{
			BroadcastAttributeChange(EAttributeType::Experience, NewValue);
		}
		else if (Attribute == GetMaxExperienceAttribute())
		{
			BroadcastAttributeChange(EAttributeType::MaxExperience, NewValue);
		}
		else if (Attribute == GetCharacterLevelAttribute())
		{
			BroadcastAttributeChange(EAttributeType::Level, NewValue);
		}
		else if (Attribute == GetAttackDamageAttribute())
		{
			BroadcastAttributeChange(EAttributeType::AttackDamage, NewValue);
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

void UDrgAttributeSet::BroadcastAttributeChange(EAttributeType AttributeType, float NewValue) const
{
	FDrgAttributeChangeMessage Message;
	Message.AttributeType = AttributeType;
	Message.NewValue = NewValue;

	UGameplayMessageSubsystem::Get(GetOwningAbilitySystemComponent()->GetWorld()).BroadcastMessage(
		DrgGameplayTags::Event_Broadcast_AttributeChanged,
		Message
	);
}
