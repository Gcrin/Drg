#include "DrgSkillCardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"

// 정적 캐시 초기화
TMap<TSoftObjectPtr<UTexture2D>, TWeakObjectPtr<UTexture2D>> UDrgSkillCardWidget::IconCache;

void UDrgSkillCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SkillButton)
	{
		SkillButton->OnClicked.AddDynamic(this, &UDrgSkillCardWidget::OnButtonClicked);
	}
	else
	{
		// 크리티컬 에러 - 더 눈에 띄게 수정
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: SkillButton이 바인딩되지 않았습니다! 블루프린트에서 BindWidget 확인 필요!"));
	}
}

void UDrgSkillCardWidget::BeginDestroy()
{
	// 비동기 로딩 핸들 정리
	if (IconLoadHandle.IsValid())
	{
		IconLoadHandle->CancelHandle();
		IconLoadHandle.Reset();
	}

	Super::BeginDestroy();
}

void UDrgSkillCardWidget::SetUpgradeChoice(const FDrgUpgradeChoice& InUpgradeChoice, int32 InSkillIndex)
{
	SkillIndex = InSkillIndex;
	bIsClickable = true;
	
	if (!SkillNameText)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: SkillNameText가 바인딩되지 않았습니다!"));
		return;
	}
	if (!SkillDescriptionText)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: SkillDescriptionText가 바인딩되지 않았습니다!"));
		return;
	}
	if (!SkillTypeText)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: SkillTypeText가 바인딩되지 않았습니다!"));
		return;
	}
	if (!SkillIcon)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: SkillIcon이 바인딩되지 않았습니다!"));
		return;
	}

	FText Name, Description, TypeText, LevelText;
	TSoftObjectPtr<UTexture2D> Icon;

	if (InUpgradeChoice.ChoiceType == EChoiceType::Evolution)
	{
		const auto& Recipe = InUpgradeChoice.EvolutionRecipe;
		if (!Recipe.EvolvedAbilityAsset)
		{
			ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: EvolutionRecipe의 EvolvedAbilityAsset이 유효하지 않습니다!"));
			return;
		}

		Name = Recipe.EvolvedAbilityAsset->AbilityName;
		FDrgAbilityLevelData Level1Data;
		if (Recipe.EvolvedAbilityAsset->GetLevelData(1, Level1Data))
		{
			Description = Level1Data.AbilityDescription;
			Icon = Level1Data.AbilityIcon;
		}
		TypeText = FText::FromString(TEXT("조합 스킬"));
		LevelText = FText::FromString(TEXT("EVOLUTION!"));
	}
	else // EChoiceType::Upgrade
	{
		if (!InUpgradeChoice.AbilityData)
		{
			ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: AbilityData가 nullptr입니다!"));
			return;
		}

		Name = InUpgradeChoice.AbilityData->AbilityName;
		FDrgAbilityLevelData LevelData;
		if (InUpgradeChoice.AbilityData->GetLevelData(InUpgradeChoice.NextLevel, LevelData))
		{
			Description = LevelData.AbilityDescription;
			Icon = LevelData.AbilityIcon;
		}

		if (InUpgradeChoice.bIsUpgrade)
		{
			TypeText = FText::Format(FText::FromString(TEXT("강화 (Lv.{0} → Lv.{1})")), InUpgradeChoice.PreviousLevel,
			                         InUpgradeChoice.NextLevel);
			if (SkillLevelText) LevelText = FText::Format(FText::FromString(TEXT("Lv.{0}")), InUpgradeChoice.NextLevel);
		}
		else
		{
			TypeText = FText::FromString(TEXT("신규 스킬"));
			if (SkillLevelText) LevelText = FText::FromString(TEXT("NEW!"));
		}
	}

	SkillNameText->SetText(Name);
	SkillDescriptionText->SetText(Description);
	SkillTypeText->SetText(TypeText);
	if (SkillLevelText) SkillLevelText->SetText(LevelText);

	LoadSkillIconAsync(Icon);
}

void UDrgSkillCardWidget::LoadSkillIconAsync(const TSoftObjectPtr<UTexture2D>& IconToLoad)
{
	// 디폴트 아이콘
	if (SkillIcon && DefaultSkillIcon) SkillIcon->SetBrushFromTexture(DefaultSkillIcon);
	// 로드할 아이콘이 유효하지 않으면 여기서 함수를 종료합니다.
	if (IconToLoad.IsNull()) return;

	// 캐시에서 먼저 확인합니다.
	if (TWeakObjectPtr<UTexture2D>* CachedIcon = IconCache.Find(IconToLoad))
	{
		if (CachedIcon->IsValid())
		{
			SkillIcon->SetBrushFromTexture(CachedIcon->Get());
			return;
		}
		IconCache.Remove(IconToLoad);
	}

	// 기존 로딩 핸들이 있다면 취소합니다.
	if (IconLoadHandle.IsValid())
	{
		IconLoadHandle->CancelHandle();
		IconLoadHandle.Reset();
	}

	// 비동기 로딩을 시작합니다.
	IconLoadHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
	   IconToLoad.ToSoftObjectPath(),
	   FStreamableDelegate::CreateUObject(this, &UDrgSkillCardWidget::OnIconLoaded, IconToLoad)
	);
}

void UDrgSkillCardWidget::OnIconLoaded(TSoftObjectPtr<UTexture2D> LoadedIconPath)
{
	if (!IconLoadHandle->GetLoadedAsset() || !IconLoadHandle.IsValid()) return;
	
	if (UTexture2D* LoadedTexture = Cast<UTexture2D>(IconLoadHandle->GetLoadedAsset()))
	{
		if (SkillIcon) SkillIcon->SetBrushFromTexture(LoadedTexture);
		IconCache.Add(LoadedIconPath, LoadedTexture);
	}

	// 핸들 정리
	if (IconLoadHandle.IsValid()) IconLoadHandle.Reset();
}

void UDrgSkillCardWidget::OnButtonClicked()
{
	// 멀티 클릭 방지 - 이미 클릭되었으면 무시
	if (!bIsClickable) return;

	// 클릭 비활성화
	bIsClickable = false;

	// 버튼 비활성화 (시각적 피드백)
	if (SkillButton) SkillButton->SetIsEnabled(false);

	OnCardClicked.Broadcast(SkillIndex);
}
