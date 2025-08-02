// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameplayStatics.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

bool UDrgGameplayStatics::AreTeamsFriendly(UAbilitySystemComponent* ASC1, UAbilitySystemComponent* ASC2)
{
	if (!ASC1 || !ASC2)
	{
		return false;
	}

	const FGameplayTag TeamTag = FGameplayTag::RequestGameplayTag(TEXT("Team"));

	FGameplayTagContainer Tags1, Tags2;
	ASC1->GetOwnedGameplayTags(Tags1);
	ASC2->GetOwnedGameplayTags(Tags2);

	const FGameplayTagContainer TeamTags1 = Tags1.Filter(FGameplayTagContainer(TeamTag));
	const FGameplayTagContainer TeamTags2 = Tags2.Filter(FGameplayTagContainer(TeamTag));

	return TeamTags1.HasAny(TeamTags2);
}

bool UDrgGameplayStatics::AreTeamsFriendly(const FGameplayTag& TeamTag1, UAbilitySystemComponent* ASC2)
{
	if (!TeamTag1.IsValid() || !ASC2)
	{
		return false;
	}

	const FGameplayTag TeamRootTag = FGameplayTag::RequestGameplayTag(TEXT("Team"));

	FGameplayTagContainer Tags2;
	ASC2->GetOwnedGameplayTags(Tags2);
	const FGameplayTagContainer TeamTags2 = Tags2.Filter(FGameplayTagContainer(TeamRootTag));

	return TeamTags2.HasTag(TeamTag1);
}
