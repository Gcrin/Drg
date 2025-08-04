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
		return FinalChoices; // 추후, 디폴트 값으로 대체
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
	
	NumChoices = FMath::Min(NumChoices, CandidateChoices.Num());
	for (int32 i = 0; i < NumChoices; ++i)
	{
		if (TotalWeight <= 0.0f) break;
        
		float RandomValue = FMath::FRandRange(KINDA_SMALL_NUMBER, TotalWeight);
		float CurrentWeightSum = 0.0f;
        
		for (int32 j = 0; j < CandidateChoices.Num(); ++j)
		{
			CurrentWeightSum += CandidateWeights[j];
			if (RandomValue <= CurrentWeightSum)
			{
				FinalChoices.Add(CandidateChoices[j]);
				TotalWeight -= CandidateWeights[j];
				CandidateWeights[j] = 0.0f;
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
