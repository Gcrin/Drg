// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameplayStatics.h"

#include "AbilitySystemComponent.h"
#include "DrgGameplayTags.h"
#include "GameplayTagContainer.h"

bool UDrgGameplayStatics::AreTeamsFriendly(UAbilitySystemComponent* ASC1, UAbilitySystemComponent* ASC2)
{
	if (!ASC1 || !ASC2)
	{
		return false;
	}

	FGameplayTagContainer Tags1, Tags2;
	ASC1->GetOwnedGameplayTags(Tags1);
	ASC2->GetOwnedGameplayTags(Tags2);

	const FGameplayTagContainer TeamTags1 = Tags1.Filter(FGameplayTagContainer(DrgGameplayTags::Team));
	const FGameplayTagContainer TeamTags2 = Tags2.Filter(FGameplayTagContainer(DrgGameplayTags::Team));

	return TeamTags1.HasAny(TeamTags2);
}

bool UDrgGameplayStatics::AreTeamsFriendly(const FGameplayTag& TeamTag1, UAbilitySystemComponent* ASC2)
{
	if (!TeamTag1.IsValid() || !ASC2)
	{
		return false;
	}

	return ASC2->HasMatchingGameplayTag(TeamTag1);
}
