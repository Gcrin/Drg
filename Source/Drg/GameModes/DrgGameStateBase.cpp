// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateBase.h"

#include "Net/UnrealNetwork.h"


ADrgGameStateBase::ADrgGameStateBase()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UDrgExperienceManagerComponent>(
		TEXT("DrgExperienceManagerComponent"));
	
	CurrentMatchState = EMatchState::WaitingToStart;
	
}

void ADrgGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void ADrgGameStateBase::OnRep_MatchState()
{
	UE_LOG(LogTemp, Warning, TEXT("Current State : %s"),
		*UEnum::GetValueAsString(TEXT("EMatchState"), CurrentMatchState));
	
	switch (CurrentMatchState)
	{
		case EMatchState::WaitingToStart:
		// 로직 추가
			break;
		case EMatchState::Inprogress:
		// 로직 추가
			break;
		case EMatchState::RoundOver:
		// 로직 추가
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("DrgGameState/FailedMatchState"));
	}
}

void ADrgGameStateBase::SetMatchState(EMatchState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentMatchState != NewState)
		{
			CurrentMatchState = NewState;
			OnRep_MatchState();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DrgGameState/이미 스테이트가 변경되었습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DrgGameState/게임모드만 스테이트를 변경할 수 있습니다."));
	}
}

EMatchState ADrgGameStateBase::GetCurrentMatchState() const
{
	return CurrentMatchState;
}

void ADrgGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADrgGameStateBase, CurrentMatchState);
}