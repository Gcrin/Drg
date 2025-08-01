// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"

ADrgPlayerCharacter::ADrgPlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false; // 충돌 테스트 비활성화
	// 탑다운 뷰를 위한 회전 값 설정
	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
}

UDataTable* ADrgPlayerCharacter::GetDataTable() const
{
	return MaxExperienceDataTable;
}

void ADrgPlayerCharacter::HandleOnLevelUp(AActor* Actor)
{
	// ToDo: 레벨 업시 표시되는 UI 구현해주세요.

	//- Test
	TArray<FDrgUpgradeChoice> AbilityChoices = GetLevelUpChoices(3);

	if (AbilityChoices.Num() > 0)
	{
		const FDrgUpgradeChoice& AutoSelectedChoice = AbilityChoices[0];
		ApplyUpgradeChoice(AutoSelectedChoice);
	}
}

void ADrgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AttributeSet)
	{
		AttributeSet->OnLevelUp.AddUObject(this, &ADrgPlayerCharacter::HandleOnLevelUp);
	}
}

TArray<FDrgUpgradeChoice> ADrgPlayerCharacter::GetLevelUpChoices(int32 NumChoices) // 선택지 개수
{
	TArray<FDrgUpgradeChoice> FinalChoices;
	if (!AbilitySystemComponent || AllAvailableAbilities.Num() == 0)
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

void ADrgPlayerCharacter::ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice)
{
	if (!SelectedChoice.AbilityData || !SelectedChoice.AbilityData->AbilityClass || !AbilitySystemComponent)
	{
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
