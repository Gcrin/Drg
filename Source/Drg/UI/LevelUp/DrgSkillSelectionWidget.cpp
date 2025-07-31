#include "DrgSkillSelectionWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "DrgSkillCardWidget.h"

void UDrgSkillSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDrgSkillSelectionWidget::ShowSkillSelection(const TArray<FDrgSkillData>& SkillOptions)
{
	if (!SkillCardContainer || !SkillCardWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UDrgSkillSelectionWidget: SkillCardContainer 또는 SkillCardWidgetClass가 설정되지 않았습니다!"));
		return;
	}

	// 기존 카드들 제거
	SkillCardContainer->ClearChildren();
	CurrentSkillOptions = SkillOptions;

	// 새 스킬 카드들 생성
	for (int32 i = 0; i < SkillOptions.Num(); ++i)
	{
		if (UDrgSkillCardWidget* SkillCard = CreateWidget<UDrgSkillCardWidget>(this, SkillCardWidgetClass))
		{
			SkillCard->SetSkillData(SkillOptions[i], i);
			SkillCard->OnCardClicked.AddUFunction(this, FName("OnSkillCardClicked"));
			SkillCardContainer->AddChild(SkillCard);
		}
	}

	// UI 표시
	AddToViewport(100); // 높은 Z-Order로 최상위에 표시
    
	// 게임 일시정지 (선택적)
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->SetPause(true);
		}
	}
}

void UDrgSkillSelectionWidget::OnSkillCardClicked(int32 SkillIndex)
{
	// 게임 일시정지 해제
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->SetPause(false);
		}
	}

	// 델리게이트 실행
	OnSkillSelected.Broadcast(SkillIndex);

	// UI 제거
	RemoveFromParent();
}
