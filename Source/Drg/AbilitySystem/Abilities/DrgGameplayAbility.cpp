// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Drg/AbilitySystem/Attributes/DrgAttributeSet.h"

void UDrgGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	StartCooldown();
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
	if (!ensureMsgf(BaseCooldown > 0.0f, TEXT("어빌리티 [%s]의 BaseCooldown 값은 0보다 커야 합니다. (현재 값: %f)"),
	                *GetName(), BaseCooldown))
	{
		return;
	}
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ensure(ASC))
	{
		return;
	}

	const float AttackSpeed = ASC->GetNumericAttribute(UDrgAttributeSet::GetAttackSpeedAttribute());

	const float FinalDuration = (AttackSpeed > 0.0f) ? (BaseCooldown / AttackSpeed) : BaseCooldown;

	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Duration")),
			                                         FinalDuration);
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToOwner(
				CurrentSpecHandle, GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
			if (!EffectHandle.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("%s: 쿨타임 적용 실패"), *GetName());
			}
		}
	}

	// 쿨다운 태그가 제거되면 OnCooldownEnded 실행하게 만드는 코드
	if (bIsOnAutoCast)
	{
		const FGameplayTagContainer* CooldownTags = GetCooldownTags();
		if (ensure(CooldownTags && CooldownTags->Num() > 0))
		{
			UAbilityTask_WaitGameplayTagRemoved* WaitCooldownTask =
				UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, CooldownTags->First());
			WaitCooldownTask->Removed.AddDynamic(this, &UDrgGameplayAbility::OnCooldownEnded);
			WaitCooldownTask->ReadyForActivation();
		}
		else
		{
			check(false && "자동시전 활성화가 되어 있지만 쿨타임이 설정되어 있지 않음");
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
		GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(CurrentSpecHandle);
		bIsOnAutoCast = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UDrgGameplayAbility: OnCooldownEnded 함수 논리 오류"));
	}

	EndAbilityAutoCheck();
}
