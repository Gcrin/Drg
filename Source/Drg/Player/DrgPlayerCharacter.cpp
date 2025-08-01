// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerCharacter.h"
#include <Drg/System/DrgGameplayStatics.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
}

UDataTable* ADrgPlayerCharacter::GetDataTable() const
{
	return MaxExperienceDataTable;
}

void ADrgPlayerCharacter::HandleOnLevelUp(AActor* Actor)
{
	// ToDo: 레벨 업시 표시되는 UI 구현해주세요.
}

void ADrgPlayerCharacter::SetTargetAcotr(AActor* pTargetActor)
{
	TargetActor = pTargetActor;
}

AActor* ADrgPlayerCharacter::GetTargetAcotr() const
{
	return TargetActor;
}

void ADrgPlayerCharacter::FindTargetActor()
{
	FVector StartLocation = this->GetTransform().GetLocation();
	TArray<AActor*> OutActors;
	//무시할 엑터
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	//Pawn엑터만 찾기
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	//특정 location 중심으로 구체를 펼쳐서 엑터들의 반환해주는 함수
	UKismetSystemLibrary::SphereOverlapActors(
		this, //UObject* WorldContext: 월드 컨텍스트를 제공하는 객체
		StartLocation, //탐지 구의 중심위치
		TraceDistance, //탐지 구의 반지름(멤버변수, 블루프린트에서 수정가능)
		ObjectTypes, //탐지할 대상
		ADrgBaseCharacter::StaticClass(), //탐지 대상 중 특정클래스 만 필터링
		IgnoreActors, //제외할 엑터 목록들
		OutActors //반환받는 배열
	);

	if (OutActors.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("No actors found!!!!!!!!!!!!!!!!!!!!!"));
		SetTargetAcotr(nullptr); // 타겟이 없으면 nullptr로 초기화
		return;
	}

	AActor* NearestEnemy = nullptr;
	float NearestDistance = TraceDistance; //멤버변수,플레이어 블루프린트에서 수정가능

	// 플레이어 캐릭터 자신의 ASC를 가져옵니다.
	UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);

	for (AActor* pTargetActor : OutActors)
	{
		if (!IsValid(pTargetActor))
			continue;

		if (pTargetActor == this) // pTargetActor로 변수명 수정
			continue;

		// 아군을 제외하는 로직 추가
		UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(pTargetActor);
		if (OwnerAsc && TargetAsc && UDrgGameplayStatics::AreTeamsFriendly(OwnerAsc, TargetAsc))
		{
			continue;
		}

		// 죽은 적을 제외하는 로직 (주석처리되어있던 것을 다시 활성화)
		ADrgBaseCharacter* TargetCharacter = Cast<ADrgBaseCharacter>(pTargetActor);
		if (IsValid(TargetCharacter) && TargetCharacter->IsDead())
		{
			continue;
		}

		// 거리 계산 및 비교 로직 
		float Distance = FVector::Dist(StartLocation, pTargetActor->GetActorLocation());

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = pTargetActor;
		}
	}

	SetTargetAcotr(NearestEnemy);
}

void ADrgPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AttributeSet)
	{
		AttributeSet->OnLevelUp.AddUObject(this, &ADrgPlayerCharacter::HandleOnLevelUp);
	}
}
