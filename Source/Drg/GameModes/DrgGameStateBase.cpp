// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateBase.h"

#include "Net/UnrealNetwork.h"


ADrgGameStateBase::ADrgGameStateBase()
{
	CurrentMatchState = EMatchState::WaitingToStart;
}

void ADrgGameStateBase::OnRep_MatchState() // 델리게이트로 함수 바인딩 -> 남한테 넘기기..ㅎ
{
	UE_LOG(LogTemp, Warning, TEXT("Current State : %s"),
		*UEnum::GetValueAsString(TEXT("EMatchState"), CurrentMatchState));
	
	// 로직 구현 // 외부 함수, 클래스로
	if (CurrentMatchState == EMatchState::WaitingToStart)
	{
		
	}
	else if (CurrentMatchState == EMatchState::Inprogress)
	{
		
	}
	else if (CurrentMatchState == EMatchState::RoundOver)
	{
		
	}
	else
	{
		// 크래시는 발생하지 않지만, 스테이트가 없는 상황이라 아무 로직도 실행 X
		UE_LOG(LogTemp, Warning,
			TEXT("DrgGameState/FailedMatchState/"));
	}
}

void ADrgGameStateBase::SetMatchState(EMatchState NewState)
{
	// 스테이트 변경은 서버가 관리 (게임 모드)
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentMatchState != NewState)
		{
			CurrentMatchState = NewState;
			OnRep_MatchState();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("이미 스테이트가 변경되었습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("게임모드만 스테이트를 변경할 수 있습니다."));
	}
}

EMatchState ADrgGameStateBase::GetCurrentMatchState() const
{
	return CurrentMatchState;
}

void ADrgGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 클라이언트에 스테이트를 복제하는 함수 // 같은 서버 내 모든 클라이언트는 스테이트 공유
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADrgGameStateBase, CurrentMatchState);
}

//--- WaitingToStart
// 메뉴UI 출력, 메뉴 BGM 재생? // UI의 버튼 클릭 로직 구현 // 시작, 상점, 전적, 설정, 종료
// 시작 : state 변경 // 상점 : ?미정? // 전적 : 인스턴스 필요
// 컨트롤러 비활성화, 마우스 커서 활성화

//--- Inprogress
// HUD 출력, HUD Update 타이머, 컨트롤러 활성화, 마우스 커서 비활성화
// 핵심 로직은 외부 클래스로 구현
// ex. ExperienceManager, UpgradeSystem, Custom GameplayEffect
// 경험치를 획득 시 확인 -> 레벨 업 -> 업그레이드 선택창 // UI 추가 및 컨트롤러 마우스 설정
// (캐릭터? 플레이어 스테이트?)사망 or 클리어 시 State 변경

//--- RoundOver
// 결과창 UI 출력, 컨트롤러 비활성화 마우스 커서 활성화
// 재시작 or 메뉴로 가기 선택 // 각각 Inprogress, WaitingToStart 스테이트로 진입