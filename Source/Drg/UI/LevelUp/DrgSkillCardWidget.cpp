#include "DrgSkillCardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UDrgSkillCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SkillButton)
	{
		SkillButton->OnClicked.AddDynamic(this, &UDrgSkillCardWidget::OnButtonClicked);
	}
}

void UDrgSkillCardWidget::SetSkillData(const FDrgSkillData& InSkillData, int32 InSkillIndex)
{
	SkillData = InSkillData;
	SkillIndex = InSkillIndex;

	// UI 업데이트
	if (SkillNameText)
	{
		SkillNameText->SetText(SkillData.SkillName);
	}

	if (SkillDescriptionText)
	{
		SkillDescriptionText->SetText(SkillData.SkillDescription);
	}

	if (SkillTypeText)
	{
		FText TypeText;
		switch (SkillData.SkillType)
		{
		case ESkillType::Active:
			TypeText = FText::FromString(TEXT("액티브"));
			break;
		case ESkillType::Passive:
			TypeText = FText::FromString(TEXT("패시브"));
			break;
		case ESkillType::Upgrade:
			TypeText = FText::FromString(TEXT("업그레이드"));
			break;
		}
		SkillTypeText->SetText(TypeText);
	}

	// 아이콘 설정 (비동기 로딩)
	if (SkillIcon && !SkillData.SkillIcon.IsNull())
	{
		if (UTexture2D* IconTexture = SkillData.SkillIcon.LoadSynchronous())
		{
			SkillIcon->SetBrushFromTexture(IconTexture);
		}
	}
}

void UDrgSkillCardWidget::OnButtonClicked()
{
	OnCardClicked.Broadcast(SkillIndex);
}
