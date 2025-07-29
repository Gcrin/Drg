// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgExperienceManagerComponent.h"

#include "Net/UnrealNetwork.h"


UDrgExperienceManagerComponent::UDrgExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	
	MaxExperience = 100.0f;
	CurrentLevel = 1;
	CurrentExperience = 0.0f;
}

void UDrgExperienceManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UDrgExperienceManagerComponent::AddExperience(float Amount)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentExperience += Amount;
		OnExperienceGained.Broadcast(CurrentExperience);
	
		while (CurrentExperience >= MaxExperience)
		{
			CurrentExperience -= MaxExperience;
			CurrentLevel++;
			OnLevelUp.Broadcast(CurrentLevel);
		}
	}
}

int32 UDrgExperienceManagerComponent::GetCurrentLevel() const
{
	return CurrentLevel;
}
float UDrgExperienceManagerComponent::GetExperience() const
{
	return CurrentExperience;
}

void UDrgExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDrgExperienceManagerComponent, CurrentLevel);
	DOREPLIFETIME(UDrgExperienceManagerComponent, CurrentExperience);
	DOREPLIFETIME(UDrgExperienceManagerComponent, MaxExperience);
}
