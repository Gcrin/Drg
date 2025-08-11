#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "DrgSkillSelectionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillSelected, int32, SkillIndex);

class UButton;
class UTextBlock;
class UImage;
class UHorizontalBox;
class UDrgSkillCardWidget;

UCLASS()
class DRG_API UDrgSkillSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 업그레이드 선택지 UI 표시
	UFUNCTION(BlueprintCallable, Category = "Skill Selection")
	void ShowUpgradeChoices(const TArray<FDrgUpgradeChoice>& UpgradeChoices);

	// 현재 업그레이드 선택지 가져오기 (PlayerCharacter에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Skill Selection")
	TArray<FDrgUpgradeChoice> GetCurrentUpgradeChoices() const { return CurrentUpgradeChoices; }
	
	// 스킬 선택 완료 시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Skill Selection")
	FOnSkillSelected OnSkillSelected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 블루프린트에서 바인딩할 UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> SkillCardContainer;

	// 스킬 카드 위젯 클래스 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UDrgSkillCardWidget> SkillCardWidgetClass;

	// UI Z-Order 설정 (기획자가 조정 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "0", ClampMax = "1000"))
	int32 UIZOrder = 100;

	// 시간 느리게 하기 배율 (0.1 = 10배 느리게)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TimeSlowRatio = 0.0f;

private:
	UFUNCTION()
	void OnSkillCardClicked(int32 SkillIndex);

	TArray<FDrgUpgradeChoice> CurrentUpgradeChoices;

	// 원래 시간 배율 저장
	float OriginalTimeDilation = 1.0f;

	// 멀티 클릭 방지
	bool bIsProcessingSelection = false;
};
