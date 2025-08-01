#include "DrgSkillCardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

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

void UDrgSkillCardWidget::SetSkillData(const FDrgSkillData& InSkillData, int32 InSkillIndex)
{
	SkillData = InSkillData;
	SkillIndex = InSkillIndex;

	// 필수 컴포넌트 체크 - 더 눈에 띄게 수정
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

	// 모든 텍스트는 즉시 설정
	SkillNameText->SetText(SkillData.SkillName);
	SkillDescriptionText->SetText(SkillData.SkillDescription);

	// SkillType은 데이터 테이블의 값을 직접 사용 (switch 제거)
	// 성장 프로그래머 또는 테크니컬 디자이너가 데이터 테이블에 미리 설정한 텍스트 사용
	if (!SkillData.SkillTypeDisplayText.IsEmpty())
	{
		SkillTypeText->SetText(SkillData.SkillTypeDisplayText);
	}
	else
	{
		// 폴백 - 기본 텍스트
		SkillTypeText->SetText(FText::FromString(TEXT("스킬")));
	}

	// 디폴트 아이콘 즉시 설정
	if (SkillIcon && DefaultSkillIcon)
	{
		SkillIcon->SetBrushFromTexture(DefaultSkillIcon);
	}

	// 실제 스킬 아이콘은 비동기 로딩
	LoadSkillIconAsync();
}

void UDrgSkillCardWidget::LoadSkillIconAsync()
{
	// 아이콘이 설정되지 않았으면 디폴트 아이콘 유지
	if (SkillData.SkillIcon.IsNull())
	{
		return;
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
		SkillData.SkillIcon.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &UDrgSkillCardWidget::OnIconLoaded)
	);
}

void UDrgSkillCardWidget::OnIconLoaded()
{
	if (!SkillIcon)
	{
		return;
	}

	// Get() 대신 LoadedAsset으로 안전하게 접근
	if (IconLoadHandle.IsValid())
	{
		if (UTexture2D* LoadedTexture = Cast<UTexture2D>(IconLoadHandle->GetLoadedAsset()))
		{
			SkillIcon->SetBrushFromTexture(LoadedTexture);
		}
		else
		{
			// 로딩 실패 시 디폴트 아이콘 유지
			UE_LOG(LogTemp, Warning, TEXT("UDrgSkillCardWidget: 스킬 아이콘 로딩 실패 - %s"), 
				   *SkillData.SkillIcon.ToString());
		}
	}

	// 핸들 정리
	if (IconLoadHandle.IsValid())
	{
		IconLoadHandle.Reset();
	}
}

void UDrgSkillCardWidget::OnButtonClicked()
{
	OnCardClicked.Broadcast(SkillIndex);
}
