// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgUpgradeComponent.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"

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
}

UDataTable* ADrgPlayerCharacter::GetDataTable() const
{
	return MaxExperienceDataTable;
}

void ADrgPlayerCharacter::HandleOnLevelUp(AActor* Actor)
{
	// ToDo: 레벨 업시 표시되는 UI 구현해주세요.

	//- Test
	TArray<FDrgUpgradeChoice> AbilityChoices = AbilityUpgradeComponent->GetLevelUpChoices(3);

	if (AbilityChoices.Num() > 0)
	{
		const FDrgUpgradeChoice& AutoSelectedChoice = AbilityChoices[0];
		AbilityUpgradeComponent->ApplyUpgradeChoice(AutoSelectedChoice);
	}
}

void ADrgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AttributeSet)
	{
		AttributeSet->OnLevelUp.AddUObject(this, &ADrgPlayerCharacter::HandleOnLevelUp);
	}
}
