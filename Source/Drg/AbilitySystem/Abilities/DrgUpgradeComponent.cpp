// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgUpgradeComponent.h"

#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "AbilitySystemInterface.h"
#include "Drg/Player/DrgPlayerCharacter.h"

UDrgUpgradeComponent::UDrgUpgradeComponent()
{
}

void UDrgUpgradeComponent::PresentLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> Choices = GetLevelUpChoices(NumChoices);
	if (Choices.Num() > 0)
	{
		OnLevelUpChoiceReady.Broadcast(Choices);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("더 이상 어빌리티를 강화하거나, 획득할 수 없습니다."));	
	}
}

void UDrgUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	IAbilitySystemInterface* AbilitySystemOwner = Cast<IAbilitySystemInterface>(GetOwner());
	// 이 컴포넌트의 오너(Owner)는 반드시 IAbilitySystemInterface를 구현해야 합니다.
	if (ensure(AbilitySystemOwner))
	{
		AbilitySystemComponent = AbilitySystemOwner->GetAbilitySystemComponent();
		ensure(AbilitySystemComponent != nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "'%s'에 부착된 DrgUpgradeComponent의 오너가 IAbilitySystemInterface를 구현하지 않았습니다! 오너의 C++ 클래스나 블루프린트에서 인터페이스를 추가해주세요."
		       ), *GetOwner()->GetName());
	}
}

TArray<FDrgUpgradeChoice> UDrgUpgradeComponent::GetLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> FinalChoices;

	if (!ensure(AbilitySystemComponent)) { return FinalChoices; }
	if (!ensureMsgf(AllAvailableAbilities.Num() > 0,
	                TEXT("UpgradeComponent에 설정된 어빌리티가 없습니다."))) { return FinalChoices; }
	
	TArray<FDrgUpgradeChoice> CandidateChoices;
	TArray<float> CandidateWeights;
	float TotalWeight = 0.0f;

	for (UDrgAbilityDataAsset* AbilityData : AllAvailableAbilities)
	{
		if (!AbilityData || AbilityData->GetMaxLevel() <= 0) continue;

		FDrgUpgradeChoice Choice;
		Choice.AbilityData = AbilityData;
		bool bIsUpgrade = false;
		int32 CurrentLevel = 0;

		if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(AbilityData))
		{
			const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);
			if (Spec && Spec->Level < AbilityData->GetMaxLevel())
			{
				bIsUpgrade = true;
				CurrentLevel = Spec->Level;
			}
		}
        
		if (bIsUpgrade || !OwnedAbilityHandles.Contains(AbilityData))
		{
			Choice.bIsUpgrade = bIsUpgrade;
			Choice.PreviousLevel = CurrentLevel;
			Choice.NextLevel = CurrentLevel + 1;

			CandidateChoices.Add(Choice);
			CandidateWeights.Add(AbilityData->SelectionWeight);
			TotalWeight += AbilityData->SelectionWeight;
		}
	}
    
	for (int32 i = 0; i < NumChoices; ++i)
	{
		if (CandidateChoices.Num() == 0 || TotalWeight <= 0.0f)	break;

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
    if (!AbilitySystemComponent) return;

    if (!SelectedChoice.AbilityData || !SelectedChoice.AbilityData->GetMaxLevel() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("선택된 어빌리티 데이터가 유효하지 않습니다."));
        return;
    }

    FDrgAbilityLevelData NextLevelData;
    if (!SelectedChoice.AbilityData->GetLevelData(SelectedChoice.NextLevel, NextLevelData))
    {
        UE_LOG(LogTemp, Warning, TEXT("강화될 어빌리티 데이터가 유효하지 않습니다."));
        return;
    }
	
    if (!SelectedChoice.bIsUpgrade || (SelectedChoice.bIsUpgrade && NextLevelData.AbilityClass != nullptr &&
        AbilitySystemComponent->FindAbilitySpecFromHandle(OwnedAbilityHandles[SelectedChoice.AbilityData])->Ability->GetClass() != NextLevelData.AbilityClass))
    {
        if (SelectedChoice.bIsUpgrade)
        {
            if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(SelectedChoice.AbilityData))
            {
                AbilitySystemComponent->ClearAbility(*FoundHandle);
                OwnedAbilityHandles.Remove(SelectedChoice.AbilityData);
            }
        }
    	
        if (NextLevelData.AbilityClass)
        {
            FGameplayAbilitySpec NewSpec(NextLevelData.AbilityClass, SelectedChoice.NextLevel, INDEX_NONE, SelectedChoice.AbilityData);
            const FGameplayAbilitySpecHandle NewHandle = AbilitySystemComponent->GiveAbility(NewSpec);
            if (NewHandle.IsValid()) OwnedAbilityHandles.Add(SelectedChoice.AbilityData, NewHandle);
        }
    }
    else
    {
       if (const FGameplayAbilitySpecHandle* FoundHandle = OwnedAbilityHandles.Find(SelectedChoice.AbilityData))
       {
          if (FGameplayAbilitySpec* SpecToUpgrade = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle))
          {
              SpecToUpgrade->Level = SelectedChoice.NextLevel;
              AbilitySystemComponent->MarkAbilitySpecDirty(*SpecToUpgrade);
          }
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
	UE_LOG(LogTemp, Warning, TEXT("삭제될 어빌리티가 존재하지 않습니다."));
}
