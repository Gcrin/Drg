// Fill out your copyright notice in the Description page of Project Settings.

#include "DrgGameStateManagerSubsystem.h"

#include "DrgGameModeBase.h"
#include "Drg/Maps/Data/DrgMapDataAsset.h"
#include "Drg/UI/DrgHUD.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

void UDrgGameStateManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 리스너 등록
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	DeathMessageListenerHandle = MessageSubsystem.RegisterListener(
		DrgGameplayTags::Event_Broadcast_ActorDied,
		this,
		&UDrgGameStateManagerSubsystem::OnDeathMessageReceived
	);
	
	// 경로 유효성 검사
	if (MapDataAssetPath.IsNull())
	{
		ensureAlwaysMsgf(false, TEXT("MapDataAssetPath가 설정되지 않았습니다! DefaultGame.ini 또는 서브시스템의 CDO를 확인해주세요."));
		return;
	}

	// 비동기 로드 요청
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(MapDataAssetPath.ToSoftObjectPath(),
	                                   FStreamableDelegate::CreateUObject(
		                                   this, &UDrgGameStateManagerSubsystem::OnMapDataLoaded));
}

void UDrgGameStateManagerSubsystem::Deinitialize()
{
	// 리스너 해제
	if (DeathMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(DeathMessageListenerHandle);
	}
	
	Super::Deinitialize();
}

void UDrgGameStateManagerSubsystem::OnMapDataLoaded()
{
	LoadedMapDataAsset = Cast<UDrgMapDataAsset>(MapDataAssetPath.Get());

	// 로드 및 캐스팅 결과 확인
	if (!ensureAlwaysMsgf(LoadedMapDataAsset != nullptr, TEXT("맵 데이터 애셋 비동기 로드 혹은 캐스팅에 실패했습니다: %s"),
	                      *MapDataAssetPath.ToString()))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("맵 데이터 애셋 비동기 로드 성공: %s"), *GetNameSafe(LoadedMapDataAsset));

	StartGame();
}

void UDrgGameStateManagerSubsystem::StartGame()
{
	ChangeState(EGameFlowState::MainMenu);
}

void UDrgGameStateManagerSubsystem::ChangeState(EGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		UE_LOG(LogTemp, Display, TEXT("이미 같은 상태입니다: %d"), (int32)NewState);
		return;
	}

	if (!LoadedMapDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("맵 데이터 에셋이 로드되지 않았습니다."));
		return;
	}

	EGameFlowState PreviousState = CurrentState;
	CurrentState = NewState;

	UE_LOG(LogTemp, Log, TEXT("상태 변경: %d → %d"), (int32)PreviousState, (int32)CurrentState);

	HandleStateChange();
}

void UDrgGameStateManagerSubsystem::ChangeStateWithResult(EGameFlowState NewState, EGameResult GameResult)
{
	CurrentGameResult = GameResult;
	ChangeState(NewState);
}

void UDrgGameStateManagerSubsystem::OnDeathMessageReceived(FGameplayTag Channel, const FDrgActorDeathMessage& Message)
{
	UE_LOG(LogTemp, Display, TEXT("UDrgGameStateManagerSubsystem::OnDeathMessageReceived"));
	
	if (ADrgGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ADrgGameModeBase>())
	{
		const EGameResult Result = GameMode->EvaluateGameEndCondition(Message.Victim);
		if (Result != EGameResult::None)
		{
			ChangeStateWithResult(EGameFlowState::PostGame, Result);
		}
	}
}

void UDrgGameStateManagerSubsystem::HandleStateChange()
{
	switch (CurrentState)
	{
	case EGameFlowState::MainMenu:
		OpenMainMenu();
		break;
	case EGameFlowState::InGame:
		OpenInGameLevel();
		break;
	case EGameFlowState::PostGame:
		ShowPostGameResults();
		break;
	case EGameFlowState::Quitting:
		QuitGame();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("처리되지 않은 상태: %d"), (int32)CurrentState);
		break;
	}

	FDrgGameStateChangeMessage Message;
	Message.NewState = CurrentState;
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(DrgGameplayTags::Event_Broadcast_StateChanged, Message); 
	UE_LOG(LogTemp, Log, TEXT("GameStateChanged: %d"), (int32)CurrentState);
}

void UDrgGameStateManagerSubsystem::OpenMainMenu()
{
	if (!LoadedMapDataAsset->MainMenuMap.IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("메인 메뉴로 이동"));
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, LoadedMapDataAsset->MainMenuMap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("메인 메뉴 맵이 설정되지 않았습니다."));
	}
}

void UDrgGameStateManagerSubsystem::OpenInGameLevel()
{
	if (!LoadedMapDataAsset->InGameMap.IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("인게임 레벨로 이동"));
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, LoadedMapDataAsset->InGameMap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("인게임 맵이 설정되지 않았습니다."));
	}
}

void UDrgGameStateManagerSubsystem::ShowPostGameResults()
{
	// HUD를 통해 게임오버 UI 표시
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ADrgHUD* HUD = PC->GetHUD<ADrgHUD>())
			{
				switch (CurrentGameResult)
				{
				case EGameResult::Victory:
					UE_LOG(LogTemp, Log, TEXT("승리!"));
					HUD->ShowGameOverUI(true);  // 승리
					break;
				case EGameResult::Defeat:
					UE_LOG(LogTemp, Log, TEXT("패배..."));
					HUD->ShowGameOverUI(false); // 패배
					break;
				case EGameResult::Draw:
					UE_LOG(LogTemp, Log, TEXT("무승부"));
					HUD->ShowGameOverUI(false); // 무승부
					break;
				default:
					UE_LOG(LogTemp, Warning, TEXT("알 수 없는 게임 결과"));
					HUD->ShowGameOverUI(false); // 기본값
					break;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DrgHUD를 찾을 수 없습니다!"));
			}
		}
	}
}

void UDrgGameStateManagerSubsystem::QuitGame()
{
	UE_LOG(LogTemp, Log, TEXT("게임 종료"));

	if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
	}
}
