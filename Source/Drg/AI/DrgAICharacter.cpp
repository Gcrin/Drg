// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAICharacter.h"
#include "Drg/AI/DrgAIController.h"


// Sets default values
ADrgAICharacter::ADrgAICharacter()
{
	CachedAIController = nullptr;
	bIsAIControlled = true;
}

// Called when the game starts or when spawned
void ADrgAICharacter::BeginPlay()
{
	Super::BeginPlay();
	// SpawnAI에서 활성화하기 전까지 비활성화
	DeactivateCharacter();
}

void ADrgAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ApplyCharacterData();
}

void ADrgAICharacter::DeactivateCharacter()
{
	Super::DeactivateCharacter();
	// 컨트롤러 입력 비활성화
	if (ADrgAIController* DrgAIController = Cast<ADrgAIController>(GetController()))
	{
		// AI 컨트롤러를 캐시에 저장
		CachedAIController = DrgAIController;
		DrgAIController->UnPossess();
	}
}

void ADrgAICharacter::ActivateCharacter()
{
	Super::ActivateCharacter();
	// 컨트롤러 입력 활성화
	if (GetController() == nullptr) // AIController가 UnPossess된 상태
	{
		// 기존에 사용했던 AI 컨트롤러가 있는지 확인하고 재사용
		if (CachedAIController)
		{
			CachedAIController->Possess(this);
			// 재사용 후 캐시 변수는 비워둠
			CachedAIController = nullptr;
		}
		else
		{
			// 기존 AIController가 없으면 새로 생성해서 Possess
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ADrgAIController* NewAIController = GetWorld()->SpawnActor<ADrgAIController>(
				AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);

			if (NewAIController)
			{
				NewAIController->Possess(this);
			}
		}
	}
}
