// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgUpgradeComponent.h"

#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "AbilitySystemInterface.h"
#include "Drg/Player/DrgPlayerCharacter.h"

UDrgUpgradeComponent::UDrgUpgradeComponent()
{
}

void UDrgUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (IAbilitySystemInterface* AbilitySystemOwner = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		AbilitySystemComponent = AbilitySystemOwner->GetAbilitySystemComponent();
		if (!AbilitySystemComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("DrgUpgradeComp/ASC를 찾을 수 없습니다."));
		}
	}
}

TArray<FDrgUpgradeChoice> UDrgUpgradeComponent::GetLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> FinalChoices;
	
	if (!ensure(AbilitySystemComponent)) return FinalChoices;
	if (!ensureMsgf(AllAvailableAbilities.Num() > 0, TEXT("GetLevelUpChoices가 빈 배열을 반환했습니다.")))
	{
		return FinalChoices;
	}
	
	TArray<FDrgUpgradeChoice> CandidateChoices;
	TArray<float> CandidateWeights;
	float TotalWeight = 0.0f;

	for (UDrgAbilityDataAsset* AbilityData : AllAvailableAbilities)
	{
		if (!AbilityData) continue;

		FDrgUpgradeChoice Choice;
		Choice.AbilityData = AbilityData;
		bool bShouldBeCandidate = false;

		if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(AbilityData))
		{
			const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);
			if (Spec && Spec->Level < AbilityData->GetMaxLevel())
			{
				bShouldBeCandidate = true;
				Choice.bIsUpgrade = true;
				Choice.PreviousLevel = Spec->Level;
				Choice.NextLevel = Spec->Level + 1;
			}
		}
		else
		{
			bShouldBeCandidate = true;
			Choice.bIsUpgrade = false;
			Choice.PreviousLevel = 0;
			Choice.NextLevel = 1;
		}
		
		if (bShouldBeCandidate)
		{
			CandidateChoices.Add(Choice);
			CandidateWeights.Add(AbilityData->SelectionWeight);
			TotalWeight += AbilityData->SelectionWeight;
		}
	}
	
	if (CandidateChoices.Num() == 0) return FinalChoices;
	NumChoices = FMath::Min(CandidateChoices.Num(), NumChoices);

	for (int32 i = 0; i < NumChoices; ++i)
	{
		if (TotalWeight <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("가중치 오류 발생. 어빌리티 가중치를 확인해주세요."));
			break;
		}

		float RandomValue = FMath::RandRange(0.0f, TotalWeight);
		float WeightSum = 0.0f;

		for (int32 j = CandidateChoices.Num() - 1; j >= 0; --j)
		{
			WeightSum += CandidateWeights[j];
			if (WeightSum > RandomValue)
			{
				FinalChoices.Add(CandidateChoices[j]);
				TotalWeight -= CandidateWeights[j];
				CandidateChoices.RemoveAtSwap(j);
				CandidateWeights.RemoveAtSwap(j);
				break;
			}
		}
	}

	return FinalChoices;
}

void UDrgUpgradeComponent::ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice)
{
	if (!ensure(AbilitySystemComponent)) return;

	if (!SelectedChoice.AbilityData)
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어에 적용할 Ability Data가 없습니다."));
		return;
	}
	if (!SelectedChoice.AbilityData->AbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어에 적용할 Ability Class가 없습니다."));
		return;
	}

	if (SelectedChoice.bIsUpgrade)
	{
		if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(SelectedChoice.AbilityData))
		{
			if (FGameplayAbilitySpec* SpecToUpgrade = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle))
			{
				SpecToUpgrade->Level = SelectedChoice.NextLevel;
				AbilitySystemComponent->MarkAbilitySpecDirty(*SpecToUpgrade);
				UE_LOG(LogTemp, Warning, TEXT("'%s' 강화! Lv.%d"),
				       *SelectedChoice.AbilityData->AbilityName.ToString(), SpecToUpgrade->Level);
			}
		}
	}
	else
	{
		FGameplayAbilitySpec NewSpec(SelectedChoice.AbilityData->AbilityClass, SelectedChoice.NextLevel, INDEX_NONE,
		                             SelectedChoice.AbilityData);
		const FGameplayAbilitySpecHandle NewHandle = AbilitySystemComponent->GiveAbility(NewSpec);
		if (NewHandle.IsValid())
		{
			OwnedAbilityHandles.Add(SelectedChoice.AbilityData, NewHandle);
			UE_LOG(LogTemp, Warning, TEXT("'%s' 획득! Lv.%d"),
			       *SelectedChoice.AbilityData->AbilityName.ToString(), SelectedChoice.NextLevel);
		}
	}
}

void UDrgUpgradeComponent::RemoveAbilityByData(UDrgAbilityDataAsset* AbilityData)
{
	if (!ensure(AbilitySystemComponent)) return;
	if (!AbilityData)
	{
		UE_LOG(LogTemp, Warning, TEXT("어빌리티 데이터가 올바르지 않습니다."));
		return;
	}

	if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(AbilityData))
	{
		AbilitySystemComponent->ClearAbility(*FoundHandle);
		OwnedAbilityHandles.Remove(AbilityData);
		return;
	}
	UE_LOG(LogTemp, Warning,TEXT("삭제될 어빌리티가 존재하지 않습니다."));
}
