// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateBase.h"

#include "Net/UnrealNetwork.h"


ADrgGameStateBase::ADrgGameStateBase()
{	
	CurrentMatchState = EMatchState::WaitingToStart;
	
}

void ADrgGameStateBase::OnRep_MatchState()
{
	UE_LOG(LogTemp, Warning, TEXT("Current State : %s"),
		*UEnum::GetValueAsString(TEXT("EMatchState"), CurrentMatchState));
	
	switch (CurrentMatchState)
	{
		case EMatchState::WaitingToStart:
		// 게임 시작 대기 상태 // 메인 메뉴
			break;
		case EMatchState::Inprogress:
		// 게임 진행 상태
			break;
		case EMatchState::RoundOver:
		// 라운드 종료 상태
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("DrgGameState/FailedMatchState"));
	}
}

void ADrgGameStateBase::SetMatchState(EMatchState NewState)
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

EMatchState ADrgGameStateBase::GetCurrentMatchState() const
{
	return CurrentMatchState;
}

void ADrgGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADrgGameStateBase, CurrentMatchState);
}