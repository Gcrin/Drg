// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgBaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Drg/AI/DrgAIController.h"
#include "Drg/Player/DrgPlayerController.h"
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
		// AI 컨트롤러를 캐시에 저장
		CachedAIController = DrgAIController;
		DrgAIController->UnPossess();
	}
	// 게임에서 숨김 
	SetActorHiddenInGame(true);
}

void ADrgBaseCharacter::ActivateCharacter()
{
	// bIsDead 초기화
	bIsDead = false;
	// 캐릭터 데이터 에셋 적용
	ApplyCharacterData();
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
	// 숨김 해제
	SetActorHiddenInGame(false);
	// ASC 재설정
	ResetAbilitySystemComponent();
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

void ADrgBaseCharacter::ResetAbilitySystemComponent()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DrgBaseCharacter] : %s의 AbilitySystemComponent 없음"), *GetName());
		return;
	}

	// 1. 모든 Active Gameplay Effect 제거
	FGameplayEffectQuery Query;
	AbilitySystemComponent->RemoveActiveEffects(Query);

	// 2. 모든 LooseGameplayTag 제거
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

	for (const FGameplayTag& Tag : OwnedTags)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
	}

	// 3. Attribute 초기화
	InitializeAttributes();

	// 4. 기본 어빌리티 부여
	if (AbilitySystemComponent->GetActivatableAbilities().Num() == 0)
	{
		GrantAbilities();
	}

	// 5. 초기 태그 다시 부여
	AbilitySystemComponent->AddLooseGameplayTags(CharacterData->InitialTags);

	// 6. 어트리뷰트 변경 델리게이트 구독 제거 후 재구독
	if (AttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMoveSpeedAttribute()
		).RemoveAll(this); // 이전 구독 제거 후
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMoveSpeedAttribute()
		).AddUObject(this, &ADrgBaseCharacter::HandleOnMoveSpeedChanged);

		GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
	}
}

void ADrgBaseCharacter::HandleOnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}
