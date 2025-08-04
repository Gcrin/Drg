// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Drg/AI/DrgAIController.h"
#include "Drg/Player/DrgPlayerController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/DrgCharacterData.h"
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

void ADrgBaseCharacter::DeactivateCharacter()
{
	// 캐릭터의 모든 충돌을 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 캐릭터의 움직임을 멈춤
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 컨트롤러 입력 비활성화
	AController* MyController = GetController();
	if (ADrgPlayerController* DrgPlayerController = Cast<ADrgPlayerController>(MyController))
	{
		DrgPlayerController->DisableInput(DrgPlayerController);
	}
	else if (ADrgAIController* DrgAIController = Cast<ADrgAIController>(MyController))
	{
		DrgAIController->BrainComponent->StopLogic(TEXT("Death"));
		DrgAIController->UnPossess();
	}

	// 게임에서 숨김 
	SetActorHiddenInGame(true);

	// 어빌리티 시스템의 모든 어빌리티와 효과를 비활성화/취소
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		// 어빌리티 중복 부여 방지
		AbilitySystemComponent->ClearAllAbilities();
	}
}

void ADrgBaseCharacter::ActivateCharacter()
{
	// bIsDead 초기화
	bIsDead = false;
	// 캐릭터의 모든 충돌을 활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 캐릭터의 움직임 활성화
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 컨트롤러 입력 활성화
	AController* MyController = GetController();

	if (ADrgPlayerController* PlayerController = Cast<ADrgPlayerController>(MyController))
	{
		PlayerController->EnableInput(PlayerController);
	}
	else if (MyController == nullptr && bIsAIControlled) // AIController가 UnPossess된 상태
	{
		// 기존 AIController가 없으면 새로 생성해서 Possess
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADrgAIController* NewAIController = GetWorld()->SpawnActor<ADrgAIController>(
			AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (NewAIController)
		{
			NewAIController->Possess(this);
			if (NewAIController->BrainComponent)
			{
				NewAIController->BrainComponent->RestartLogic();
			}
		}
	}
	else if (ADrgAIController* AIController = Cast<ADrgAIController>(MyController))
	{
		if (AIController->BrainComponent)
		{
			AIController->BrainComponent->RestartLogic();
		}
	}
	// 캐릭터 데이터 에셋 적용
	ApplyCharacterData();
	// 숨김 해제
	SetActorHiddenInGame(false);
}

void ADrgBaseCharacter::ApplyCharacterData()
{
	if (CharacterData && CharacterData->IsValidData())
	{
		if (USkeletalMeshComponent* MeshComponent = GetMesh())
		{
			if (CharacterData->SkeletalMesh)
			{
				MeshComponent->SetSkeletalMesh(CharacterData->SkeletalMesh);

				// 2. 데이터 애셋의 머티리얼 오버라이드 배열을 순회하며 하나씩 적용
				for (int32 i = 0; i < CharacterData->MaterialOverrides.Num(); ++i)
				{
					UMaterialInterface* MaterialToApply = CharacterData->MaterialOverrides[i].LoadSynchronous();

					// 디자이너의 설정 실수를 대비한 if + UE_LOG 처리
					// 배열에 항목은 있지만, 실제 애셋이 할당되지 않았을 경우를 대비
					if (MaterialToApply)
					{
						MeshComponent->SetMaterial(i, MaterialToApply);
					}
					else
					{
						// 로그를 남겨서 어떤 데이터 애셋의 몇 번째 슬롯이 비어있는지 명확히 알려준다
						UE_LOG(LogTemp, Warning, TEXT(
							       "ADrgBaseCharacter: '%s' 데이터 애셋의 MaterialOverrides 배열 [%d] 슬롯이 비어있습니다. 원본 머티리얼이 사용됩니다."
						       ), *CharacterData->GetName(), i);
					}
				}
			}

			if (CharacterData->AnimClass)
			{
				MeshComponent->SetAnimInstanceClass(CharacterData->AnimClass);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgBaseCharacter] : %s에 할당된 CharacterData가 유효하지 않습니다!"), *GetName());
	}
}

void ADrgBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	ActivateCharacter();
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

		InitializeAttributes();

		AbilitySystemComponent->AddLooseGameplayTags(CharacterData->InitialTags);

		if (AttributeSet)
		{
			AttributeSet->OnDeath.AddUObject(this, &ADrgBaseCharacter::HandleOnDeath);

			// MoveSpeed 어트리뷰트의 값 변경 델리게이트에 OnMoveSpeedAttributeChanged 함수를 구독
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				AttributeSet->GetMoveSpeedAttribute()
			).AddUObject(this, &ADrgBaseCharacter::HandleOnMoveSpeedChanged);

			GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
		}

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
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgBaseCharacter] : %s의 AbilitySystemComponent가 유효하지 않습니다!"), *GetName());
		return;
	}

	if (!CharacterData || !CharacterData->IsValidData())
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgBaseCharacter] : %s에 CharacterData가 할당되지 않았습니다!"), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		CharacterData->StatsInitializerEffect, 1, EffectContext);

	if (SpecHandle.IsValid())
	{
		for (const FAttributeInitializationData& InitData : CharacterData->InitialAttributes)
		{
			FString TagString = FString::Printf(TEXT("Stat.%s"), *InitData.Attribute.AttributeName);
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(*TagString),
			                                         InitData.BaseValue);
		}

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ADrgBaseCharacter::GrantAbilities()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgBaseCharacter] : %s의 AbilitySystemComponent가 유효하지 않습니다!"), *GetName());
		return;
	}

	if (!CharacterData)
	{
		UE_LOG(LogTemp, Error, TEXT("[DrgBaseCharacter] : %s에 CharacterData가 할당되지 않았습니다!"), *GetName());
		return;
	}
	if (CharacterData->IsValidData())
	{
		for (TSubclassOf<UGameplayAbility>& Ability : CharacterData->DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
		}
	}
}

void ADrgBaseCharacter::HandleOnDeath(AActor* DeadActor)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (AbilitySystemComponent)
	{
		// GA_Drg_Death 어빌리티 태그 정의
		FGameplayTag DeathEventTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
		// GameplayEventData 생성 및 몽타주 선택 태그 포함
		FGameplayEventData EventData;
		EventData.Instigator = this;
		EventData.Target = this;
		EventData.TargetTags.AddTag(CharacterData->DeathTypeTag);
		// GA_Drg_Death 활성화
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, DeathEventTag, EventData);
	}

	UE_LOG(LogTemp, Warning, TEXT("[DrgBaseCharacter] : %s 캐릭터가 사망했습니다."), *GetName());
}

void ADrgBaseCharacter::HandleOnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}
