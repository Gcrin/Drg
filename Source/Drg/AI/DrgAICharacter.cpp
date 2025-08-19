// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAICharacter.h"

#include "Components/CapsuleComponent.h"
#include "Drg/AI/DrgAIController.h"
#include "Drg/Character/Data/DrgCharacterData.h"
#include "Drg/System/DrgGameplayTags.h"

#define COLLISION_MONSTER ECC_GameTraceChannel1
// Sets default values
ADrgAICharacter::ADrgAICharacter()
{
	CachedAIController = nullptr;
	bIsAIControlled = true;

	// 화면에 보이지 않는 Mesh는 애니메이션 Tick을 멈춰 CPU 최적화
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
}

FGameplayTag ADrgAICharacter::GetDropTag() const
{
	return CharacterData ? CharacterData->DropTag : FGameplayTag::EmptyTag;
}

// Called when the game starts or when spawned
void ADrgAICharacter::BeginPlay()
{
	Super::BeginPlay();
	// SpawnAI에서 활성화하기 전까지 비활성화
	DeactivateCharacter();
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
		// 컨트롤러의 레퍼런스 확실하게 끊기
		DrgAIController->SetPawn(nullptr);
	}
}

void ADrgAICharacter::ActivateCharacter()
{
	Super::ActivateCharacter();

	// 몬스터 채널 충돌 설정
	if (AbilitySystemComponent->HasMatchingGameplayTag(DrgGameplayTags::Team_Enemy_Elite))
	{
		// 엘리트 몬스터일 경우 몬스터 충돌 무시
		GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_MONSTER, ECR_Ignore);
	}
	else
	{
		// 엘리트 몬스터 아니면 다른 몬스터와 충돌
		GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_MONSTER, ECR_Block);
	}

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

void ADrgAICharacter::OnDeathCleanup()
{
	Super::OnDeathCleanup();
	AbilitySystemComponent->CancelAbilities();
	AbilitySystemComponent->ClearAllAbilities();

	OnAIDied.Broadcast(this);
}
