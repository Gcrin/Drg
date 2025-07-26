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

	// 서버에서 GAS 초기화
	InitializeAttributes();
}

void ADrgBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트에서 GAS 초기화
	InitializeAttributes();
}

void ADrgBaseCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && GetPlayerState())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);

		check(DefaultAttributes);

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DefaultAttributes, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		GrantAbilities();
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
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
	}
}
