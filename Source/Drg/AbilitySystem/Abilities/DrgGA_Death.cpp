// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGA_Death.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Drg/Character/Data/DrgCharacterData.h"

UDrgGA_Death::UDrgGA_Death()
{
	FGameplayTagContainer AssetTagContainer;
	AssetTagContainer.AddTag(DrgGameplayTags::Event_Death);
	SetAssetTags(AssetTagContainer);
}

void UDrgGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                              bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ADrgBaseCharacter* AvatarCharacter = CastChecked<ADrgBaseCharacter>(ActorInfo->AvatarActor);
	if (AvatarCharacter)
	{
		AvatarCharacter->OnDeathCleanup();
	}
}

FOwnerData UDrgGA_Death::GetOwnerData() const
{
	FOwnerData OwnerData;
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		OwnerData.OwningCharacter = CastChecked<ADrgBaseCharacter>(AvatarActor);
		OwnerData.AbilitySystemComponent = OwnerData.OwningCharacter->GetAbilitySystemComponent();
		auto CharacterData = OwnerData.OwningCharacter->GetCharacterData();
		if (CharacterData)
		{
			TObjectPtr<UAnimMontage>* FoundMontage = CharacterData->AbilityMontages.Find(CharacterData->DeathTypeTag);
			if (FoundMontage)
			{
				OwnerData.DeathMontage = *FoundMontage;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("해당 태그에 해당하는 몽타주를 찾지 못했습니다 : %s"),
				       *CharacterData->DeathTypeTag.ToString());
			}
		}
	}
	return OwnerData;
}
