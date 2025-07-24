// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ADrgBaseCharacter::ADrgBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent); // 루트 컴포넌트에 부착
	SpringArmComponent->TargetArmLength = 800.0f; // 카메라 거리
	SpringArmComponent->bUsePawnControlRotation = false; // 폰의 회전을 사용하지 않음
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false; // 충돌 테스트 비활성화
	// 탑다운 뷰를 위한 회전 값 설정
	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent); // 스프링암에 부착
	CameraComponent->bUsePawnControlRotation = false; // 폰의 회전을 사용하지 않음
}

void ADrgBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}
