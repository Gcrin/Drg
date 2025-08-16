#include "DrgSkillCardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"
#include "PaperSprite.h"

// 정적 캐시 초기화
TMap<TSoftObjectPtr<UPaperSprite>, TWeakObjectPtr<UPaperSprite>> UDrgSkillCardWidget::IconCache;

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

	// 현재 로드된 아이콘 경로가 유효하면 캐시에서 제거
	if (!CurrentLoadedIconPath.IsNull())
	{
		IconCache.Remove(CurrentLoadedIconPath);
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

	FDrgSkillCardUIData UIData;
	TSoftObjectPtr<UPaperSprite> Icon;

	UIData.ChoiceType = InUpgradeChoice.ChoiceType;

	if (InUpgradeChoice.AbilityData)
	{
		UIData.Name = InUpgradeChoice.AbilityData->AbilityName;
		UIData.bIsUpgrade = InUpgradeChoice.bIsUpgrade;
		UIData.PreviousLevel = InUpgradeChoice.PreviousLevel;
		UIData.NextLevel = InUpgradeChoice.NextLevel;

		FDrgAbilityLevelData LevelData;
		if (InUpgradeChoice.AbilityData->GetLevelData(InUpgradeChoice.NextLevel, LevelData))
		{
			UIData.Description = LevelData.AbilityDescription;
			Icon = LevelData.AbilityIcon;
		}
	}
	LoadSkillIconAsync(Icon);
	OnUpdateSkillCardDisplay(UIData);
}

void UDrgSkillCardWidget::LoadSkillIconAsync(const TSoftObjectPtr<UPaperSprite>& IconToLoad)
{
	// 디폴트 아이콘
	if (SkillIcon && DefaultSkillIcon)
	{
		FSlateBrush DefaultBrush;
		DefaultBrush.SetResourceObject(DefaultSkillIcon);
		DefaultBrush.SetImageSize(IconSize);
		SkillIcon->SetBrush(DefaultBrush);
	}
	// 로드할 아이콘이 유효하지 않으면 여기서 함수를 종료합니다.
	if (IconToLoad.IsNull())
		return;

	// 캐시에서 먼저 확인합니다.
	if (TWeakObjectPtr<UPaperSprite>* CachedIcon = IconCache.Find(IconToLoad))
	{
		if (CachedIcon->IsValid())
		{
			FSlateBrush NewBrush;
			NewBrush.SetResourceObject(CachedIcon->Get());
			NewBrush.SetImageSize(IconSize);
			SkillIcon->SetBrush(NewBrush);

			CurrentLoadedIconPath = IconToLoad;
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

void UDrgSkillCardWidget::OnIconLoaded(TSoftObjectPtr<UPaperSprite> LoadedIconPath)
{
	if (!IconLoadHandle.IsValid() || !IconLoadHandle->GetLoadedAsset())
		return;

	if (UPaperSprite* LoadedSprite = Cast<UPaperSprite>(IconLoadHandle->GetLoadedAsset()))
	{
		if (SkillIcon)
		{
			FSlateBrush NewBrush;
			NewBrush.SetResourceObject(LoadedSprite);
			// 여기에 원하는 고정된 크기 값을 설정
			NewBrush.SetImageSize(IconSize); // 예시: 100x100 픽셀로 고정
			//     
			// NewBrush.SetImageSize(FVector2D(LoadedSprite->GetSourceSize()));
			SkillIcon->SetBrush(NewBrush);
		}
		// IconCache.Add(LoadedIconPath, LoadedSprite);
		IconCache.Emplace(LoadedIconPath, LoadedSprite);
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
