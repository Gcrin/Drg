// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerCharacter.h"

#include "DrgPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/Abilities/DrgUpgradeComponent.h"
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
		AbilityUpgradeComponent->PresentLevelUpChoices();
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
}

void ADrgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AttributeSet)
	{
		AttributeSet->OnLevelUp.AddUObject(this, &ADrgPlayerCharacter::HandleOnLevelUp);
	}
}
