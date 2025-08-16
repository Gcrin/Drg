// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgUpgradeComponent.h"

#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "AbilitySystemInterface.h"
#include "Data/DrgUpgradeDataCollection.h"
#include "Data/DrgEvolutionData.h"
#include "Drg/Player/DrgPlayerCharacter.h"
#include "Drg/UI/DrgSkillInformation.h"

UDrgUpgradeComponent::UDrgUpgradeComponent()
{
	static ConstructorHelpers::FObjectFinder<UDrgUpgradeDataCollection> AbilityDataCollectionFinder(
		TEXT("/Script/Drg.DrgUpgradeDataCollection'/Game/Core/Data/DA_Drg_UpgradeData.DA_Drg_UpgradeData'"));
	if (AbilityDataCollectionFinder.Succeeded()) AbilityCollectionData = AbilityDataCollectionFinder.Object;
	else UE_LOG(LogTemp, Error, TEXT("AbilityCollectionData: Failed to find"));

	static ConstructorHelpers::FObjectFinder<UDrgEvolutionDataAsset> EvolutionDataFinder(
		TEXT("/Script/Drg.DrgEvolutionDataAsset'/Game/Core/Data/DA_Drg_EvolutionData.DA_Drg_EvolutionData'"));
	if (EvolutionDataFinder.Succeeded()) EvolutionData = EvolutionDataFinder.Object;
	else UE_LOG(LogTemp, Error, TEXT("EvolutionData: Failed to find"));
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

bool UDrgUpgradeComponent::PresentLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> Choices = GetLevelUpChoices(NumChoices);
	if (Choices.Num() > 0)
	{
		OnLevelUpChoiceReady.Broadcast(Choices);
		return true;
	}
	UE_LOG(LogTemp, Display, TEXT("더 이상 어빌리티를 강화하거나, 획득할 수 없습니다."));
	OnLevelUpChoiceReady.Broadcast(Choices);
	return false;
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

void UDrgUpgradeComponent::UpdateEquippedSkills()
{
	EquippedSkills.Empty();

	for (const auto& Ability : OwnedAbilityHandles)
	{
		if (Ability.Key && Ability.Value.IsValid())
		{
			if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Ability.Value))
			{
				FDrgSkillInformation SkillInfo;
				SkillInfo.SkillData = Ability.Key;
				SkillInfo.Level = Spec->Level;
				SkillInfo.bIsEvolution = EvolvedSkills.Contains(Ability.Key);
				EquippedSkills.Add(SkillInfo);
			}
		}
	}
	for (const auto& Effect : ActiveEffectHandles)
	{
		if (const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent->GetActiveGameplayEffect(Effect.Value))
		{
			FDrgSkillInformation SkillInfo;
			SkillInfo.SkillData = Effect.Key;
			SkillInfo.Level = ActiveEffect->Spec.GetLevel();
			SkillInfo.bIsEvolution = EvolvedSkills.Contains(Effect.Key);
			EquippedSkills.Add(SkillInfo);
		}
	}
	OnEquippedSkillsChanged.Broadcast();
}

void UDrgUpgradeComponent::ExecuteEvolution(const FDrgEvolutionRecipe& Recipe)
{
	for (const FDrgIngredientInfo& IngredientInfo : Recipe.Ingredients)
	{
		if (IngredientInfo.bShouldBeRemoved && IngredientInfo.IngredientAsset)
		{
			RemoveAbilityByData(IngredientInfo.IngredientAsset);
		}
	}

	FDrgUpgradeChoice EvolvedChoice;
	EvolvedChoice.AbilityData = Recipe.EvolvedAbilityAsset;
	EvolvedChoice.bIsUpgrade = false;
	EvolvedChoice.PreviousLevel = 0;
	EvolvedChoice.NextLevel = 1;

	FDrgAbilityLevelData LevelData;
	if (Recipe.EvolvedAbilityAsset && Recipe.EvolvedAbilityAsset->GetLevelData(1, LevelData))
	{
		if (LevelData.UpgradeType == EUpgradeType::Ability) { UpgradeAbility(EvolvedChoice); }
		else if (LevelData.UpgradeType == EUpgradeType::Effect) { UpgradeEffect(EvolvedChoice); }
	}
}

TArray<FDrgEvolutionRecipe> UDrgUpgradeComponent::GetPossibleEvolutions() const
{
	TArray<FDrgEvolutionRecipe> PossibleEvolutions;
	if (!EvolutionData || !AbilitySystemComponent) { return PossibleEvolutions; }

	for (const FDrgEvolutionRecipe& Recipe : EvolutionData->EvolutionRecipes)
	{
		if (Recipe.Ingredients.IsEmpty() || !Recipe.EvolvedAbilityAsset) continue;
		if (OwnedAbilityHandles.Contains(Recipe.EvolvedAbilityAsset) ||
			ActiveEffectHandles.Contains(Recipe.EvolvedAbilityAsset))
			continue;

		bool bHasAllMaxLevelIngredients = true;
		for (const FDrgIngredientInfo& IngredientInfo : Recipe.Ingredients)
		{
			if (!IngredientInfo.IngredientAsset)
			{
				bHasAllMaxLevelIngredients = false;
				break;
			}

			int32 IngredientLevel = 0;
			if (const FGameplayAbilitySpecHandle* FoundAbilitySpecHandle = OwnedAbilityHandles.Find(
				IngredientInfo.IngredientAsset))
			{
				if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(
					*FoundAbilitySpecHandle))
				{
					IngredientLevel = Spec->Level;
				}
			}
			else if (const FActiveGameplayEffectHandle* FoundEffectHandle = ActiveEffectHandles.Find(
				IngredientInfo.IngredientAsset))
			{
				if (const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent->GetActiveGameplayEffect(
					*FoundEffectHandle))
				{
					IngredientLevel = ActiveEffect->Spec.GetLevel();
				}
			}

			if (IngredientLevel < IngredientInfo.IngredientAsset->GetMaxLevel())
			{
				bHasAllMaxLevelIngredients = false;
				break;
			}
		}
		if (bHasAllMaxLevelIngredients) { PossibleEvolutions.Add(Recipe); }
	}
	return PossibleEvolutions;
}

TArray<FDrgUpgradeChoice> UDrgUpgradeComponent::GetLevelUpChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> FinalChoices;

	if (!ensure(AbilitySystemComponent)) { return FinalChoices; }
	if (!ensureMsgf(AbilityCollectionData && AbilityCollectionData->UpgradeData.Num() > 0,
	                TEXT("UpgradeComponent에 설정된 어빌리티가 없습니다."))) { return FinalChoices; }

	// 진화 가능한 선택지 우선 반환
	TArray<FDrgEvolutionRecipe> PossibleEvolutions = GetPossibleEvolutions();

	if (PossibleEvolutions.Num() > NumChoices)
	{
		while (PossibleEvolutions.Num() > NumChoices)
		{
			PossibleEvolutions.RemoveAtSwap(FMath::RandRange(0, PossibleEvolutions.Num() - 1));
		}
	}
	for (const auto& Recipe : PossibleEvolutions)
	{
		FDrgUpgradeChoice EvolutionChoice;
		EvolutionChoice.AbilityData = Recipe.EvolvedAbilityAsset;
		EvolutionChoice.PreviousLevel = -1; // 진화는 -1로 구분

		EvolutionChoice.EvolutionRecipe = Recipe;
		EvolutionChoice.ChoiceType = EChoiceType::Evolution;
		FinalChoices.Add(EvolutionChoice);
	}

	int32 RemainingChoices = NumChoices - FinalChoices.Num();
	if (RemainingChoices > 0)
	{
		const int32 CurrentOwnedAbilityCount = OwnedAbilityHandles.Num();

		TArray<FDrgUpgradeChoice> CandidateChoices;
		TArray<float> CandidateWeights;
		float TotalWeight = 0.0f;

		for (const auto& AbilityData : AbilityCollectionData->UpgradeData)
		{
			if (!AbilityData || AbilityData->GetMaxLevel() <= 0) continue;
			if (RemovedAbilities.Contains(AbilityData)) continue;

			int32 CurrentLevel = 0;
			if (const FGameplayAbilitySpecHandle* FoundAbilitySpecHandle = OwnedAbilityHandles.Find(AbilityData))
			{
				if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(
					*FoundAbilitySpecHandle))
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
			if (CurrentLevel >= AbilityData->GetMaxLevel()) continue;

			// 다음 레벨의 데이터를 가져와 업그레이드 타입을 확인
			FDrgAbilityLevelData NextLevelData;
			if (!AbilityData->GetLevelData(CurrentLevel + 1, NextLevelData)) continue;

			// 새로운 업그레이드를 얻으려 하지만, 이미 최대 개수를 소유한 경우 후보에서 제외
			const bool bIsNewAcquisition = (CurrentLevel == 0);
			if (bIsNewAcquisition && NextLevelData.UpgradeType ==
				EUpgradeType::Ability && CurrentOwnedAbilityCount >= MaxAcquirableAbilityCount)
				continue;

			FDrgUpgradeChoice Choice;
			Choice.AbilityData = AbilityData;
			Choice.bIsUpgrade = !bIsNewAcquisition;
			Choice.PreviousLevel = CurrentLevel;
			Choice.NextLevel = CurrentLevel + 1;
			Choice.ChoiceType = EChoiceType::Upgrade;

			CandidateChoices.Add(Choice);
			CandidateWeights.Add(AbilityData->SelectionWeight);
			TotalWeight += AbilityData->SelectionWeight;
		}

		for (int32 i = 0; i < RemainingChoices; ++i)
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
	}

	return FinalChoices;
}

void UDrgUpgradeComponent::ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice)
{
	if (!AbilitySystemComponent) return;
	if (!SelectedChoice.AbilityData) return;

	if (ADrgPlayerCharacter* PlayerCharacter = Cast<ADrgPlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->SetCharacterAppearance(
			SelectedChoice.AbilityData->NewMesh,
			SelectedChoice.AbilityData->NewMaterials
		);
	}

	if (SelectedChoice.ChoiceType == EChoiceType::Evolution)
	{
		ExecuteEvolution(SelectedChoice.EvolutionRecipe);
		EvolvedSkills.Add(SelectedChoice.AbilityData);
	}
	else
	{
		FDrgAbilityLevelData NextLevelData;
		if (!SelectedChoice.AbilityData->GetLevelData(SelectedChoice.NextLevel, NextLevelData)) return;

		if (NextLevelData.UpgradeType == EUpgradeType::Ability) UpgradeAbility(SelectedChoice);
		else if (NextLevelData.UpgradeType == EUpgradeType::Effect) UpgradeEffect(SelectedChoice);
	}

	UpdateEquippedSkills();
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
		RemovedAbilities.Add(AbilityData);

		UpdateEquippedSkills();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("삭제될 어빌리티가 존재하지 않습니다."));
}

TArray<FDrgUpgradeChoice> UDrgUpgradeComponent::StartAbilityChoices(int32 NumChoices)
{
	TArray<FDrgUpgradeChoice> FinalChoices;

	if (!ensure(AbilitySystemComponent)) { return FinalChoices; }
	if (!ensureMsgf(AbilityCollectionData && AbilityCollectionData->UpgradeData.Num() > 0,
	                TEXT("UpgradeComponent에 설정된 어빌리티가 없습니다."))) { return FinalChoices; }

	TArray<FDrgUpgradeChoice> CandidateChoices;
	TArray<float> CandidateWeights;
	float TotalWeight = 0.0f;

	for (const auto& UpgradeData : AbilityCollectionData->UpgradeData)
	{
		if (!UpgradeData || UpgradeData->GetMaxLevel() <= 0) continue;
		if (OwnedAbilityHandles.Contains(UpgradeData)) continue;

		FDrgAbilityLevelData NextLevelData;
		if (!UpgradeData->GetLevelData(1, NextLevelData)) continue;
		if (NextLevelData.UpgradeType != EUpgradeType::Ability) continue;

		FDrgUpgradeChoice Choice;
		Choice.AbilityData = UpgradeData;
		Choice.bIsUpgrade = false;
		Choice.PreviousLevel = 0;
		Choice.NextLevel = 1;

		CandidateChoices.Add(Choice);
		CandidateWeights.Add(UpgradeData->SelectionWeight);
		TotalWeight += UpgradeData->SelectionWeight;
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
