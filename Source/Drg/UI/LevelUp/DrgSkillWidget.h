// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DrgSkillWidget.generated.h"

class UImage;
class UTextBlock;
class UDrgAbilityDataAsset;

/**
 *  @brief HUD, GameOverUI 등에 표시될 스킬 정보(아이콘, 이름, 레벨)
 */
UCLASS()
class DRG_API UDrgSkillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAbilityInfo(const UDrgAbilityDataAsset* SkillData, int32 Level, bool bIsEvolution);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
};
