// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Drg/AbilitySystem/DrgAbilitySystemComponent.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"

ADrgBaseCharacter::ADrgBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

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
