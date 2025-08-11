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
	UpgradeChoice = InUpgradeChoice;
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

	// AbilityData 유효성 검사
	if (!UpgradeChoice.AbilityData)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillCardWidget: AbilityData가 nullptr입니다!"));
		return;
	}

	// 스킬 정보 설정 (AbilityDataAsset에서 가져오기)
	FDrgAbilityLevelData LevelData;
	if (UpgradeChoice.AbilityData->GetLevelData(UpgradeChoice.NextLevel, LevelData))
	{
		if (SkillNameText) SkillNameText->SetText(UpgradeChoice.AbilityData->AbilityName);
		if (SkillDescriptionText) SkillDescriptionText->SetText(LevelData.AbilityDescription);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UDrgSkillCardWidget: 유효하지 않은 레벨 데이터가 전달되었습니다. Level: %d"), UpgradeChoice.NextLevel);
		if (SkillNameText) SkillNameText->SetText(FText::FromString(TEXT("Unknown")));
		if (SkillDescriptionText) SkillDescriptionText->SetText(FText::FromString(TEXT("Error loading ability data.")));
	}

	// 스킬 타입 설정 (업그레이드 여부에 따라)
	FText TypeText;
	if (UpgradeChoice.bIsUpgrade)
	{
		TypeText = FText::Format(FText::FromString(TEXT("강화 (Lv.{0} → Lv.{1})")), 
								 UpgradeChoice.PreviousLevel, 
								 UpgradeChoice.NextLevel);
	}
	else
	{
		TypeText = FText::FromString(TEXT("신규 스킬"));
	}
	SkillTypeText->SetText(TypeText);

	// 레벨 텍스트 설정 (임시/선택사항)
	if (SkillLevelText)
	{
		if (UpgradeChoice.bIsUpgrade)
		{
			SkillLevelText->SetText(FText::Format(FText::FromString(TEXT("Lv.{0}")), UpgradeChoice.NextLevel));
		}
		else
		{
			SkillLevelText->SetText(FText::FromString(TEXT("NEW!")));
		}
	}

	// 디폴트 아이콘 즉시 설정
	if (SkillIcon && DefaultSkillIcon)
	{
		SkillIcon->SetBrushFromTexture(DefaultSkillIcon);
	}

	// 실제 스킬 아이콘은 비동기 로딩 (캐싱 포함)
	LoadSkillIconAsync();
}

void UDrgSkillCardWidget::LoadSkillIconAsync()
{
	// 소프트 포인터가 유효하지 않으면 디폴트 아이콘 유지
	FDrgAbilityLevelData LevelData;
	if (!UpgradeChoice.AbilityData || !UpgradeChoice.AbilityData->GetLevelData(UpgradeChoice.NextLevel, LevelData) || LevelData.AbilityIcon.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("UDrgSkillCardWidget: 아이콘이 설정되지 않았거나 데이터가 유효하지 않습니다."));
		return;
	}

	// 캐시에서 먼저 확인
	if (TWeakObjectPtr<UTexture2D>* CachedIcon = IconCache.Find(LevelData.AbilityIcon))
	{
		if (CachedIcon->IsValid())
		{
			// 캐시에서 즉시 사용
			SkillIcon->SetBrushFromTexture(CachedIcon->Get());
			return;
		}
		else
		{
			// 캐시에 있지만 GC(가비지 컬렉션)된 경우 제거
			IconCache.Remove(LevelData.AbilityIcon);
		}
	}

	// 기존 로딩 취소
	if (IconLoadHandle.IsValid())
	{
		IconLoadHandle->CancelHandle();
		IconLoadHandle.Reset();
	}

	// 비동기 로딩 시작
	UAssetManager& AssetManager = UAssetManager::Get();
	IconLoadHandle = AssetManager.GetStreamableManager().RequestAsyncLoad(
		LevelData.AbilityIcon.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &UDrgSkillCardWidget::OnIconLoaded)
	);

	UE_LOG(LogTemp, Log, TEXT("UDrgSkillCardWidget: 비동기 아이콘 로딩 시작 - %s"), 
		   *UpgradeChoice.AbilityData->AbilityName.ToString());
}

void UDrgSkillCardWidget::OnIconLoaded()
{
	if (!SkillIcon || !UpgradeChoice.AbilityData || !IconLoadHandle.IsValid())
	{
		return;
	}
	
	FDrgAbilityLevelData LevelDataForIcon;
	if (!UpgradeChoice.AbilityData->GetLevelData(UpgradeChoice.NextLevel, LevelDataForIcon))
	{
		return;
	}
	
	if (UTexture2D* LoadedTexture = Cast<UTexture2D>(IconLoadHandle->GetLoadedAsset()))
	{
		SkillIcon->SetBrushFromTexture(LoadedTexture);
		IconCache.Add(LevelDataForIcon.AbilityIcon, LoadedTexture);
	}
	else
	{
		if (DefaultSkillIcon)
		{
			SkillIcon->SetBrushFromTexture(DefaultSkillIcon);
		}
		UE_LOG(LogTemp, Warning, TEXT("스킬 아이콘 로딩 실패! %s DataAsset의 AbilityIcon 경로(%s)를 확인하세요."),
			*UpgradeChoice.AbilityData->GetName(),
			*LevelDataForIcon.AbilityIcon.ToString());
	}

	// 핸들 정리
	if (IconLoadHandle.IsValid())
	{
		IconLoadHandle.Reset();
	}
}

void UDrgSkillCardWidget::OnButtonClicked()
{
	// 멀티 클릭 방지 - 이미 클릭되었으면 무시
	if (!bIsClickable)
	{
		return;
	}

	// 클릭 비활성화
	bIsClickable = false;

	// 버튼 비활성화 (시각적 피드백)
	if (SkillButton)
	{
		SkillButton->SetIsEnabled(false);
	}

	OnCardClicked.Broadcast(SkillIndex);
}
