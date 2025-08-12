// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgUpgradeComponent.h"

#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "AbilitySystemInterface.h"
#include "Data/DrgUpgradeDataCollection.h"
#include "Drg/Player/DrgPlayerCharacter.h"

UDrgUpgradeComponent::UDrgUpgradeComponent()
{
	static ConstructorHelpers::FObjectFinder<UDrgUpgradeDataCollection> AbilityDataCollectionFinder(
		TEXT("/Script/Drg.DrgUpgradeDataCollection'/Game/Core/Data/DA_Drg_UpgradeData.DA_Drg_UpgradeData'"));

	if (AbilityDataCollectionFinder.Succeeded())
	{
		AbilityCollectionData = AbilityDataCollectionFinder.Object;
	}
	else UE_LOG(LogTemp, Error, TEXT("AbilityCollectionData: Failed to find"));
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

void UDrgUpgradeComponent::UpgradeAbility(const FDrgUpgradeChoice& SelectedChoice)
{
	FDrgAbilityLevelData NextLevelData;
	if (!SelectedChoice.AbilityData->GetLevelData(SelectedChoice.NextLevel, NextLevelData))
	{
		UE_LOG(LogTemp, Warning, TEXT("강화될 어빌리티의 레벨 데이터가 유효하지 않습니다."));
		return;
	}

	if (!NextLevelData.AbilityClass)
	{
		UE_LOG(LogTemp, Error, TEXT("다음 레벨의 어빌리티 클래스가 유효하지 않습니다."));
		return;
	}

	if (const FGameplayAbilitySpecHandle* FoundHandlePtr = OwnedAbilityHandles.Find(SelectedChoice.AbilityData))
	{
		AbilitySystemComponent->ClearAbility(*FoundHandlePtr);
		OwnedAbilityHandles.Remove(SelectedChoice.AbilityData);
	}

	FGameplayAbilitySpec NewSpec(NextLevelData.AbilityClass, SelectedChoice.NextLevel, INDEX_NONE,
	                             SelectedChoice.AbilityData);
	const FGameplayAbilitySpecHandle NewHandle = AbilitySystemComponent->GiveAbility(NewSpec);

	if (NewHandle.IsValid())
	{
		OwnedAbilityHandles.Add(SelectedChoice.AbilityData, NewHandle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("새 어빌리티 '%s' (Lv.%d) 부여에 실패했습니다."),
		       *SelectedChoice.AbilityData->GetName(), SelectedChoice.NextLevel);
	}
}

void UDrgUpgradeComponent::UpgradeEffect(const FDrgUpgradeChoice& SelectedChoice)
{
	FDrgAbilityLevelData NextLevelData;
	if (!SelectedChoice.AbilityData->GetLevelData(SelectedChoice.NextLevel, NextLevelData)) return;

	if (!NextLevelData.EffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("다음 레벨의 이펙트 클래스가 유효하지 않습니다."));
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		NextLevelData.EffectClass, SelectedChoice.NextLevel, EffectContext);
	FActiveGameplayEffectHandle NewHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
		*SpecHandle.Data.Get());

	if (NewHandle.IsValid())
	{
		ActiveEffectHandles.Add(SelectedChoice.AbilityData, NewHandle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("새 이펙트 '%s' (Lv.%d) 부여에 실패했습니다."),
		       *SelectedChoice.AbilityData->GetName(), SelectedChoice.NextLevel);
	}
}

TArray<FDrgUpgradeChoice> UDrgUpgradeComponent::GetLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> FinalChoices;

	if (!ensure(AbilitySystemComponent)) { return FinalChoices; }
	if (!ensureMsgf(AbilityCollectionData && AbilityCollectionData->UpgradeData.Num() > 0,
	                TEXT("UpgradeComponent에 설정된 어빌리티가 없습니다."))) { return FinalChoices; }

	// 현재 소유한 어빌리티의 개수
	const int32 CurrentOwnedAbilityCount = OwnedAbilityHandles.Num();

	TArray<FDrgUpgradeChoice> CandidateChoices;
	TArray<float> CandidateWeights;
	float TotalWeight = 0.0f;

	for (const auto& AbilityData : AbilityCollectionData->UpgradeData)
	{
		if (!AbilityData || AbilityData->GetMaxLevel() <= 0) continue;

		int32 CurrentLevel = 0;

		// 어빌리티와 이펙트 중 현재 소유한 업그레이드가 있는지 확인하여 현재 레벨을 가져옵니다.
		if (const FGameplayAbilitySpecHandle* FoundAbilityHandle = OwnedAbilityHandles.Find(AbilityData))
		{
			if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(
				*FoundAbilityHandle))
			{
				CurrentLevel = Spec->Level;
			}
		}
		else if (const FActiveGameplayEffectHandle* FoundEffectHandle = ActiveEffectHandles.Find(AbilityData))
		{
			if (const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent->GetActiveGameplayEffect(
				*FoundEffectHandle))
			{
				CurrentLevel = ActiveEffect->Spec.GetLevel();
			}
		}

		// 이미 최대 레벨인 업그레이드는 후보에서 제외
		if (CurrentLevel >= AbilityData->GetMaxLevel())
		{
			continue;
		}
		// 다음 레벨의 데이터를 가져와 업그레이드 타입을 확인
		FDrgAbilityLevelData NextLevelData;
		if (!AbilityData->GetLevelData(CurrentLevel + 1, NextLevelData))
		{
			continue;
		}
		// 새로운 업그레이드를 얻으려 하지만, 이미 최대 개수를 소유한 경우 후보에서 제외
		const bool bIsNewAcquisition = (CurrentLevel == 0);
		if (bIsNewAcquisition && NextLevelData.UpgradeType ==
			EUpgradeType::Ability && CurrentOwnedAbilityCount >= MaxAcquirableAbilityCount - 1)
		{
			continue;
		}

		FDrgUpgradeChoice Choice;
		Choice.AbilityData = AbilityData;
		Choice.bIsUpgrade = !bIsNewAcquisition;
		Choice.PreviousLevel = CurrentLevel;
		Choice.NextLevel = CurrentLevel + 1;

		CandidateChoices.Add(Choice);
		CandidateWeights.Add(AbilityData->SelectionWeight);
		TotalWeight += AbilityData->SelectionWeight;
	}

	for (int32 i = 0; i < NumChoices; ++i)
	{
		if (CandidateChoices.Num() == 0 || TotalWeight <= 0.0f) break;

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
	if (!SelectedChoice.AbilityData) return;

	FDrgAbilityLevelData NextLevelData;
	if (!SelectedChoice.AbilityData->GetLevelData(SelectedChoice.NextLevel, NextLevelData)) return;

	if (NextLevelData.UpgradeType == EUpgradeType::Ability) UpgradeAbility(SelectedChoice);
	else if (NextLevelData.UpgradeType == EUpgradeType::Effect) UpgradeEffect(SelectedChoice);
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
