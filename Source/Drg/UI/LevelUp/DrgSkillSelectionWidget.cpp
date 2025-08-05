#include "DrgSkillSelectionWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "DrgSkillCardWidget.h"
#include "Kismet/GameplayStatics.h"

void UDrgSkillSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 필수 컴포넌트 체크 - 더 눈에 띄게 수정
	if (!SkillCardContainer)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillSelectionWidget: SkillCardContainer가 바인딩되지 않았습니다! 블루프린트에서 BindWidget 확인 필요!"));
	}
    
	if (!SkillCardWidgetClass)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillSelectionWidget: SkillCardWidgetClass가 설정되지 않았습니다! 블루프린트에서 설정 필요!"));
	}
}

void UDrgSkillSelectionWidget::ShowUpgradeChoices(const TArray<FDrgUpgradeChoice>& UpgradeChoices)
{
	if (!SkillCardContainer)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillSelectionWidget: SkillCardContainer가 없어서 UI를 생성할 수 없습니다!"));
		return;
	}
    
	if (!SkillCardWidgetClass)
	{
		ensureAlwaysMsgf(false, TEXT("UDrgSkillSelectionWidget: SkillCardWidgetClass가 없어서 스킬 카드를 생성할 수 없습니다!"));
		return;
	}

	if (UpgradeChoices.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDrgSkillSelectionWidget: 표시할 업그레이드 선택지가 없습니다!"));
		return;
	}

	// 기존 카드들 제거
	SkillCardContainer->ClearChildren();
	CurrentUpgradeChoices = UpgradeChoices;

	// 새 스킬 카드들 생성
	for (int32 i = 0; i < UpgradeChoices.Num(); ++i)
	{
		if (UDrgSkillCardWidget* SkillCard = CreateWidget<UDrgSkillCardWidget>(this, SkillCardWidgetClass))
		{
			SkillCard->SetUpgradeChoice(UpgradeChoices[i], i);
			SkillCard->OnCardClicked.AddDynamic(this, &UDrgSkillSelectionWidget::OnSkillCardClicked);
			SkillCardContainer->AddChild(SkillCard);
		}
		else
		{
			ensureAlwaysMsgf(false, TEXT("UDrgSkillSelectionWidget: 스킬 카드 위젯 생성 실패! 인덱스: %d"), i);
		}
	}

	// UI 표시
	AddToViewport(UIZOrder); // 기획자가 설정 가능한 Z-Order
    
	// 시간 느리게 하기 (pause 대신)
	if (UWorld* World = GetWorld())
	{
		OriginalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(World);
		UGameplayStatics::SetGlobalTimeDilation(World, TimeSlowRatio);
	}
}

void UDrgSkillSelectionWidget::OnSkillCardClicked(int32 SkillIndex)
{
	// 시간 배율 복원
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, OriginalTimeDilation);
	}

	// 델리게이트 실행
	OnSkillSelected.Broadcast(SkillIndex);

	// UI 제거
	RemoveFromParent();
}
