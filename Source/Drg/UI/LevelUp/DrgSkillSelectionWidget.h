#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DrgSkillData.h"
#include "DrgSkillSelectionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillSelected, int32, SkillIndex);

class UButton;
class UTextBlock;
class UImage;
class UHorizontalBox;

UCLASS()
class DRG_API UDrgSkillSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스킬 선택 UI 표시
	UFUNCTION(BlueprintCallable, Category = "Skill Selection")
	void ShowSkillSelection(const TArray<FDrgSkillData>& SkillOptions);

	// 스킬 선택 완료 시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Skill Selection")
	FOnSkillSelected OnSkillSelected;

protected:
	virtual void NativeConstruct() override;

	// 블루프린트에서 바인딩할 UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> SkillCardContainer;

	// 스킬 카드 위젯 클래스 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> SkillCardWidgetClass;

private:
	UFUNCTION()
	void OnSkillCardClicked(int32 SkillIndex);

	TArray<FDrgSkillData> CurrentSkillOptions;
};
