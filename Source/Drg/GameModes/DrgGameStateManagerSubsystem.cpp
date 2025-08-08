// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateManagerSubsystem.h"

#include "Drg/Maps/Data/DrgMapDataAsset.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"


void UDrgGameStateManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDrgGameStateManagerSubsystem::StartLoadingSequence()
{
	checkf(MapDataAssetPath.IsValid(),
	       TEXT("UDrgGameStateManagerSubsystem 설정에 MapDataAssetPath가 지정되지 않았습니다. DefaultGame.ini 파일을 확인해주세요."));

	UWorld* CurrentWorld = GetWorld();
	if (!ensure(CurrentWorld)) return;

	// 로딩 화면 맵으로 먼저 이동
	UGameplayStatics::OpenLevel(CurrentWorld, TEXT("L_LoadingMap"));

	// 백그라운드에서 실제 데이터 에셋 로딩 요청
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(
		MapDataAssetPath.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &UDrgGameStateManagerSubsystem::OnMapDataLoaded)
	);
}

void UDrgGameStateManagerSubsystem::OnMapDataLoaded()
{
	LoadedMapDataAsset = MapDataAssetPath.Get();
	if (ensure(LoadedMapDataAsset))
	{
		ChangeState(EGameFlowState::MainMenu);
	}
}

void UDrgGameStateManagerSubsystem::ChangeState(EGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		UE_LOG(LogTemp, Display, TEXT("동일한 스테이트로는 변경할 수 없습니다."));
		return;
	}

	checkf(LoadedMapDataAsset, TEXT("LoadedMapDataAsset가 유효하지 않습니다. OnMapDataLoaded()가 호출되지 않았거나 실패했습니다."));

	CurrentState = NewState;

	switch (CurrentState)
	{
	case EGameFlowState::MainMenu:
		HandleMainMenuState();
		break;
	case EGameFlowState::InGame:
		HandleInGameState();
		break;
	case EGameFlowState::PostGame:
		HandlePostGameState();
		break;
	case EGameFlowState::Quitting:
		HandleQuittingState();
		break;
	default:
		break;
	}
}

void UDrgGameStateManagerSubsystem::HandleMainMenuState()
{
	UE_LOG(LogTemp, Display, TEXT("메인 메뉴로 이동"));
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LoadedMapDataAsset->MainMenuMap);
}

void UDrgGameStateManagerSubsystem::HandleInGameState()
{
	UE_LOG(LogTemp, Display, TEXT("게임 레벨로 이동"));
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LoadedMapDataAsset->InGameMap);
}

void UDrgGameStateManagerSubsystem::HandlePostGameState()
{
	UE_LOG(LogTemp, Display, TEXT("게임 결과창으로 이동"));
}

void UDrgGameStateManagerSubsystem::HandleQuittingState()
{
	UE_LOG(LogTemp, Display, TEXT("게임 종료"));
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}
