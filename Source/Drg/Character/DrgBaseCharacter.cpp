// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Drg/AbilitySystem/DrgAbilitySystemComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

ADrgBaseCharacter::ADrgBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UDrgAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UDrgAttributeSet>(TEXT("AttributeSet"));
}

bool ADrgBaseCharacter::IsDead() const
{
	return bIsDead;
}

UAbilitySystemComponent* ADrgBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TObjectPtr<UDrgAttributeSet> ADrgBaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ADrgBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADrgBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		// PlayerState가 있는 경우 (플레이어)
		if (GetPlayerState())
		{
			AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
		}
		// PlayerState가 없는 경우 (AI)
		else
		{
			// Owner는 자기 자신, Avatar는 컨트롤러가 조종하는 폰(자기 자신)으로 초기화
			AbilitySystemComponent->InitAbilityActorInfo(this, this);
		}

		if (AttributeSet)
		{
			AttributeSet->OnDeath.AddUObject(this, &ADrgBaseCharacter::HandleOnDeath);
		}

		// 속성 적용 및 어빌리티 부여
		InitializeAttributes();
		GrantAbilities();
	}
}

void ADrgBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent && GetPlayerState())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
	}
}

void ADrgBaseCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent)
	{
		check(DefaultAttributes);

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DefaultAttributes, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ADrgBaseCharacter::GrantAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
	}
}

void ADrgBaseCharacter::HandleOnDeath(AActor* DeadActor)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// 캐릭터의 모든 충돌을 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 캐릭터의 움직임을 멈춤
	GetCharacterMovement()->StopMovementImmediately();
	// 어빌리티 시스템의 모든 어빌리티와 효과를 비활성화/취소
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	UE_LOG(LogTemp, Warning, TEXT("[DrgBaseCharacter] : %s 캐릭터가 사망했습니다."), *GetName());

	// TODO: 죽음 애니메이션 몽타주 재생 필요
	// PlayAnimMontage(...)

	// TODO: AI 캐릭터는 추후 오브젝트 풀링 방식으로 교체 예정
	// 현재는 애니메이션 처리 미구현 상태이므로 임시 삭제 방식 사용
	// 향후: 죽음 애니메이션 재생 완료 후 삭제되도록 변경 필요
	SetLifeSpan(0.1f);
}
