#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DrgSkillData.h"
#include "DrgSkillCardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardClicked, int32, SkillIndex);

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class DRG_API UDrgSkillCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스킬 데이터 설정
	UFUNCTION(BlueprintCallable, Category = "Skill Card")
	void SetSkillData(const FDrgSkillData& InSkillData, int32 InSkillIndex);

	// 카드 클릭 시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Skill Card")
	FOnCardClicked OnCardClicked;

protected:
	virtual void NativeConstruct() override;

	// 블루프린트에서 바인딩할 UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkillButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillTypeText;

private:
	UFUNCTION()
	void OnButtonClicked();

	FDrgSkillData SkillData;
	int32 SkillIndex;
};
