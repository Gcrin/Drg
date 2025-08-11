// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerCharacter.h"

#include "DrgPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgUpgradeComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"
// UI 관련 include 추가
#include "Drg/UI/LevelUp/DrgSkillSelectionWidget.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "Kismet/GameplayStatics.h"

ADrgPlayerCharacter::ADrgPlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false; // 충돌 테스트 비활성화
	// 탑다운 뷰를 위한 회전 값 설정
	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	AbilityUpgradeComponent = CreateDefaultSubobject<UDrgUpgradeComponent>(TEXT("AbilityUpgradeComponent"));

	bIsAIControlled = false;
}

UDataTable* ADrgPlayerCharacter::GetDataTable() const
{
	return MaxExperienceDataTable;
}

void ADrgPlayerCharacter::HandleOnLevelUp(AActor* Actor)
{
	if (AbilityUpgradeComponent)
	{
		UpgradeCount++;
		UE_LOG(LogTemp, Warning, TEXT("%d"), UpgradeCount);

		if (UpgradeCount == 1) AbilityUpgradeComponent->PresentLevelUpChoices();
	}
}

void ADrgPlayerCharacter::DeactivateCharacter()
{
	Super::DeactivateCharacter();
	// 컨트롤러 입력 비활성화
	if (ADrgPlayerController* DrgPlayerController = Cast<ADrgPlayerController>(GetController()))
	{
		DrgPlayerController->DisableInput(DrgPlayerController);
	}
}

void ADrgPlayerCharacter::ActivateCharacter()
{
	Super::ActivateCharacter();
	// 컨트롤러 입력 활성화
	if (ADrgPlayerController* PlayerController = Cast<ADrgPlayerController>(GetController()))
	{
		PlayerController->EnableInput(PlayerController);
	}
}

void ADrgPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	ActivateCharacter();

	// UI 위젯 생성 및 델리게이트 바인딩
	if (SkillSelectionWidgetClass && AbilityUpgradeComponent)
	{
		if (APlayerController* PC = GetController<APlayerController>())
		{
			SkillSelectionWidget = CreateWidget<UDrgSkillSelectionWidget>(PC, SkillSelectionWidgetClass);
			
			if (SkillSelectionWidget)
			{
				// 컴포넌트의 선택지 준비 완료 → UI 표시
				AbilityUpgradeComponent->OnLevelUpChoiceReady.AddDynamic(
					SkillSelectionWidget, &UDrgSkillSelectionWidget::ShowUpgradeChoices);
				
				// UI의 스킬 선택 완료 → 캐릭터 처리  
				SkillSelectionWidget->OnSkillSelected.AddDynamic(
					this, &ADrgPlayerCharacter::OnSkillSelected);
			}
			else
			{
				ensureAlwaysMsgf(false, TEXT("ADrgPlayerCharacter: 스킬 선택 위젯 생성 실패!"));
			}
		}
	}
	else if (!SkillSelectionWidgetClass)
	{
		ensureAlwaysMsgf(false, TEXT("ADrgPlayerCharacter: SkillSelectionWidgetClass가 설정되지 않았습니다! 블루프린트에서 설정하세요!"));
	}
}

void ADrgPlayerCharacter::OnSkillSelected(int32 SkillIndex)
{
	if (!SkillSelectionWidget || !AbilityUpgradeComponent)
	{
		return;
	}

	// 시간 배율 복원 (UI 제거 전에 먼저)
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);  // 정상 속도로 복원
	}

	// UI에서 현재 선택지 가져오기
	TArray<FDrgUpgradeChoice> CurrentChoices = SkillSelectionWidget->GetCurrentUpgradeChoices();
	
	if (CurrentChoices.IsValidIndex(SkillIndex))
	{
		// 선택된 스킬 적용
		const FDrgUpgradeChoice& SelectedChoice = CurrentChoices[SkillIndex];
		AbilityUpgradeComponent->ApplyUpgradeChoice(SelectedChoice);
		UpgradeCount--;
	}

	if (UpgradeCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(LevelUpTimerHandle, this,
			&ADrgPlayerCharacter::CheckLevelUp, 0.5f, false);
	}
}

void ADrgPlayerCharacter::CheckLevelUp()
{
	if (UpgradeCount > 0)
	{
		AbilityUpgradeComponent->PresentLevelUpChoices();
	}
	UE_LOG(LogTemp, Warning, TEXT("남은 업그레이드: %d"), UpgradeCount);
}

void ADrgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AttributeSet)
	{
		AttributeSet->OnLevelUp.AddUObject(this, &ADrgPlayerCharacter::HandleOnLevelUp);
	}
}
