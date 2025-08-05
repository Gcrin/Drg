// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Drg/System/DrgGameplayTags.h"

UDrgGameplayAbility::UDrgGameplayAbility()
{
	CooldownMagnitudeTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Duration"));
}

void UDrgGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	StartCooldown();
}

bool UDrgGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        FGameplayTagContainer* OptionalRelevantTags) const
{
	UAbilitySystemComponent* const ASC = ActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	// 우리가 블루프린트에 직접 설정한 CooldownTags 변수를 사용하여 체크
	if (CooldownTags.Num() > 0)
	{
		if (ASC->HasAnyMatchingGameplayTags(CooldownTags))
		{
			// 쿨타임 태그를 가지고 있다면, 어빌리티 사용 실패
			return false;
		}
	}

	// 쿨타임 태그가 없다면, 어빌리티 사용 성공
	return true;
}

void UDrgGameplayAbility::EndTaskAutoCheck()
{
	bIsTaskFinished = true;
	if (!bIsOnAutoCast)
	{
		EndAbility(CurrentSpecHandle, GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}

void UDrgGameplayAbility::StartCooldown()
{
	// 쿨타임 이펙트 클래스 가져오기
	if (!ensure(CooldownGameplayEffectClass != nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Ability [%s]: CooldownGameplayEffectClass가 설정되지 않았습니다!"), *GetName());
		return;
	}

	// 이펙트 Spec 생성
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		CooldownGameplayEffectClass, GetAbilityLevel());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	// 어빌리티에 설정된 고유 CooldownTags를 Spec에 동적으로 주입
	SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	// BaseCooldown 값을 GE에 전달
	SpecHandle.Data->SetSetByCallerMagnitude(CooldownMagnitudeTag, BaseCooldown);

	// 최종적으로 만들어진 Spec을 소유자에게 적용
	FActiveGameplayEffectHandle EffectHandle =
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, GetCurrentActorInfo(), GetCurrentActivationInfo(),
		                               SpecHandle);

	// 쿨다운 태그가 제거되면 OnCooldownEnded 실행하게 만드는 코드
	if (bIsOnAutoCast)
	{
		if (ensureMsgf(CooldownTags.Num() > 0, TEXT(
			               "Ability [%s]: 자동시전이 활성화되었지만 Cooldown Tags가 설정되지 않았습니다."), *GetName()))
		{
			UAbilityTask_WaitGameplayTagRemoved* WaitCooldownTask =
				UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, CooldownTags.First());
			WaitCooldownTask->Removed.AddDynamic(this, &UDrgGameplayAbility::OnCooldownEnded);
			WaitCooldownTask->ReadyForActivation();
		}
	}
}

void UDrgGameplayAbility::EndAbilityAutoCheck()
{
	if (!bIsOnAutoCast && bIsTaskFinished)
	{
		EndAbility(CurrentSpecHandle, GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}

void UDrgGameplayAbility::OnCooldownEnded()
{
	if (bIsOnAutoCast)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ensure(ASC))
		{
			if (!ASC->HasMatchingGameplayTag(DrgGameplayTags::State_Dead))
			{
				ASC->TryActivateAbility(CurrentSpecHandle);
			}
			bIsOnAutoCast = false;
		}
	}

	EndAbilityAutoCheck();
}
