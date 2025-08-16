// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgSkillWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"

void UDrgSkillWidget::SetAbilityInfo(const UDrgAbilityDataAsset* SkillData, int32 Level, bool bIsEvolution)
{
	if (!SkillData) return;

	if (LevelText && !bIsEvolution)
	{
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Lv. %d"), Level)));
	}
	
	if (EvolutionBorder && !bIsEvolution)
	{
		EvolutionBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	FDrgAbilityLevelData LevelData;
	if (SkillData->GetLevelData(Level, LevelData))
	{
		UTexture2D* LoadedIcon = LevelData.AbilityIcon.LoadSynchronous();
		if (SkillIcon && LoadedIcon) SkillIcon->SetBrushFromTexture(LoadedIcon);
	}
}
