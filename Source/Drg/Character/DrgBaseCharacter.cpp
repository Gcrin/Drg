// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

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

UAbilitySystemComponent* ADrgBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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
