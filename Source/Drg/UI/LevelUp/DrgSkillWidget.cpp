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
		UPaperSprite* LoadedIcon = LevelData.AbilityIcon.LoadSynchronous();
		//UPaperSprite를 불러올수있도록 수정
		if (SkillIcon && LoadedIcon)
		{
			//SkillIcon->SetBrushFromTexture(LoadedIcon);
			FSlateBrush NewBrush;
			NewBrush.SetResourceObject(LoadedIcon);
			//이미지 사이즈 안해주면 스프라이트 크기를 따라가서 강제로 fix
			NewBrush.SetImageSize(FVector2D(32.f, 32.f));
			SkillIcon->SetBrush(NewBrush);
		}
	}
}
