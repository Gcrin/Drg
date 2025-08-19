// Fill out your copyright notice in the Description page of Project Settings.

#include "DrgSpawnAI.h"
#include "NavigationSystem.h"
#include "DrgWaveTableRow.h"
#include "Components/CapsuleComponent.h"
#include "Drg/AI/DrgAICharacter.h"
#include "Drg/Character/Data/DrgCharacterData.h"
#include "Drg/GameModes/DrgPlayerState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADrgSpawnAI::ADrgSpawnAI()
{
	MinDistance = 1000.f;
	MaxDistance = 2000.f;
	TotalSpawnCount = 200;
}

void ADrgSpawnAI::SetCurrentWaveNumber(int32 NewWaveNumber)
{
	CurrentWaveNumber = NewWaveNumber;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ADrgPlayerState* PlayerState = PlayerController->GetPlayerState<ADrgPlayerState>())
		{
			const bool bIsLast = (CurrentWaveNumber >= LastWaveNumber);
			PlayerState->SetbIsLastWave(bIsLast);
			PlayerState->SetCurrentWaveNumber(CurrentWaveNumber);
		}
	}
	StopSpawnTimer();
	StartSpawnTimer();
}

void ADrgSpawnAI::SetNextWave()
{
	SetCurrentWaveNumber(CurrentWaveNumber + 1);
}

void ADrgSpawnAI::BeginPlay()
{
	Super::BeginPlay();
	// 웨이브 스폰 카운트 배열 초기화
	TArray<FDrgWaveTableRow*> Rows;
	WaveDataTable->GetAllRows<FDrgWaveTableRow>(TEXT("WaveTableContext"), Rows);
	LastWaveNumber = Rows.Num();
	CurrentWaveSpawnCount.SetNum(LastWaveNumber + 1);

	InitializePool();
	SetNextWave();

	GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this, &ADrgSpawnAI::SetNextWave, WaveChangeTime, true);
}

void ADrgSpawnAI::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void ADrgSpawnAI::InitializePool()
{
	for (int32 i = 0; i < TotalSpawnCount; i++)
	{
		FVector SpawnLocation;
		if (FindSafeRandomPointInNav(SpawnLocation))
		{
			// SpawnLocation 로그 출력
			UE_LOG(LogTemp, Warning, TEXT("DrgSpawnAI:: 스폰 위치: %s"), *SpawnLocation.ToString());
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ADrgAICharacter* SpawnedAI = GetWorld()->SpawnActor<ADrgAICharacter>(
				AICharacterClass,
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);
			if (SpawnedAI)
			{
				InActiveAIPool.Add(SpawnedAI);
				SpawnedAI->OnAIDied.AddDynamic(this, &ADrgSpawnAI::ReturnAIToPool);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("SpawnedAI is NULL!"));
			}
		}
	}
}

void ADrgSpawnAI::ReturnAIToPool(class ADrgAICharacter* DeadAI)
{
	if (ActiveAIPool.Contains(DeadAI))
	{
		ActiveAIPool.Remove(DeadAI);
		InActiveAIPool.Add(DeadAI);
		CurrentWaveSpawnCount[CurrentWaveNumber].Remove(DeadAI);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("풀에 AI 캐릭터를 반환하려 했지만, Active 풀에서 찾을 수 없습니다. 풀링 로직에 오류가 있을 수 있습니다. 대상 캐릭터: %s"),
		       *DeadAI->GetName());
	}
}

void ADrgSpawnAI::DeactivateAll()
{
	TArray<ADrgAICharacter*> AIToDeactivate = ActiveAIPool;
	for (auto AI : AIToDeactivate)
	{
		if (ActiveAIPool.Contains(AI))
		{
			AI->DeactivateCharacter();
			AI->OnDeathCleanup();
		}
	}
}

void ADrgSpawnAI::StartSpawnTimer()
{
	FDrgWaveTableRow* CurrentWaveRow = GetCurrentWaveDataRow(CurrentWaveNumber);
	if (!CurrentWaveRow) return;

	CurrentWaveSpawnCount[CurrentWaveNumber].Empty();

	// 마지막 웨이브(보스전) 시작 전에 활성화되어있는 몬스터 정리
	if (CurrentWaveNumber >= LastWaveNumber)
	{
		// 웨이브 타이머 clear
		if (NextWaveTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(NextWaveTimerHandle);
			NextWaveTimerHandle.Invalidate();
			UE_LOG(LogTemp, Warning,
			       TEXT("DrgSpawnAI:: 웨이브 타이머 종료"));
		}
		DeactivateAll();
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ADrgSpawnAI::SpawnAILoop,
		CurrentWaveRow->SpawnInterval,
		true,
		1.0f
	);
	UE_LOG(LogTemp, Warning,
	       TEXT("DrgSpawnAI:: %d 웨이브 시작"), CurrentWaveNumber);
}

void ADrgSpawnAI::StopSpawnTimer()
{
	if (SpawnTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		SpawnTimerHandle.Invalidate();
	}
}

bool ADrgSpawnAI::FindSafeRandomPointInNav(FVector& ResultLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return false;
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return false;
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	for (int32 i = 0; i < MaxTryCount; i++)
	{
		float RandomDistance = FMath::RandRange(MinDistance, MaxDistance);
		float RandomAngle = FMath::RandRange(0.f, 2 * PI);
		FVector RandomOffset = FVector(
			RandomDistance * FMath::Cos(RandomAngle),
			RandomDistance * FMath::Sin(RandomAngle),
			0.f
		);

		FVector TestLocation = PlayerLocation + RandomOffset;
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(TestLocation, NavLocation))
		{
			// NavMesh 위 값 저장
			ResultLocation = NavLocation.Location;
			return true;
		}
	}
	return false;
}

FDrgWaveTableRow* ADrgSpawnAI::GetCurrentWaveDataRow(int32 WaveNumber)
{
	if (!WaveDataTable) return nullptr;

	// 행 이름을 웨이브 번호로 변환
	const FName RowName = FName(*FString::FromInt(WaveNumber));
	static const FString ContextString(TEXT("GetCurrentWaveDataRow"));

	FDrgWaveTableRow* CurrentRow = WaveDataTable->FindRow<FDrgWaveTableRow>(RowName, ContextString);

	if (!CurrentRow)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("DrgSpawnAI::WaveNumber %d에 해당하는 열을 찾을 수 없습니다. [해결 방법: DT_Wave에 해당 웨이브 정보를 입력해주세요]"), WaveNumber);
	}
	return CurrentRow;
}

TObjectPtr<UDrgCharacterData> ADrgSpawnAI::GetRandomAICharacterData()
{
	FDrgWaveTableRow* CurrentWaveRow = GetCurrentWaveDataRow(CurrentWaveNumber);
	float TotalChance = 0;
	for (auto& p : CurrentWaveRow->AIData)
	{
		TotalChance += p.Value;
	}

	const float RandomValue = FMath::FRandRange(0.f, TotalChance);
	float AccumulatedChance = 0;
	for (auto& p : CurrentWaveRow->AIData)
	{
		AccumulatedChance += p.Value;
		if (RandomValue <= AccumulatedChance)
		{
			return p.Key;
		}
	}

	return nullptr;
}

TObjectPtr<class ADrgAICharacter> ADrgSpawnAI::SpawnAIFromPool()
{
	FVector SpawnLocation;
	if (FindSafeRandomPointInNav(SpawnLocation))
	{
		if (InActiveAIPool.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inactive 풀이 비어있습니다. 새로운 몬스터를 생성할 수 없습니다."));
			return nullptr;
		}
		ADrgAICharacter* SpawnedAI = InActiveAIPool.Pop();
		ActiveAIPool.Add(SpawnedAI);
		CurrentWaveSpawnCount[CurrentWaveNumber].Add(SpawnedAI);
		SpawnedAI->SetCharacterData(GetRandomAICharacterData());
		SpawnedAI->SetActorLocation(
			SpawnLocation + FVector(0, 0, SpawnedAI->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
		SpawnedAI->ActivateCharacter();

		return SpawnedAI;
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("DrgSpawnAI::SpawnLocation을 찾을 수 없어 AI 스폰을 실패했습니다."));
		return nullptr;
	}
}

void ADrgSpawnAI::SpawnAILoop()
{
	if (ActiveAIPool.Num() >= TotalSpawnCount)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("DrgSpawnAI::스폰된 AI가 TotalSpawnCount를 초과했습니다 %d. 더이상 스폰되지 않습니다."), ActiveAIPool.Num());
		return;
	}

	FDrgWaveTableRow* CurrentWaveRow = GetCurrentWaveDataRow(CurrentWaveNumber);
	for (int32 i = 0; i < CurrentWaveRow->SpawnCount; i++)
	{
		if (CurrentWaveSpawnCount[CurrentWaveNumber].Num() >= CurrentWaveRow->MaxSpawnCount)
		{
			break;
		}
		ADrgAICharacter* SpawnedAI = SpawnAIFromPool();
	}
}
