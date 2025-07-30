// Fill out your copyright notice in the Description page of Project Settings.

#include "DrgSpawnAI.h"
#include "NavigationSystem.h"
#include "DrgWaveTableRow.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "Drg/Character/Data/DrgCharacterData.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADrgSpawnAI::ADrgSpawnAI()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

	MinDistance = 1000.f;
	MaxDistance = 2000.f;
	TotalSpawnCount = 200;
}

void ADrgSpawnAI::InitializePool()
{
	for (int32 i = 0; i < TotalSpawnCount; i++)
	{
		FVector SpawnLocation;
		if (FindSafeRandomPointInNav(SpawnLocation))
		{
			ADrgBaseCharacter* SpawnedAI = GetWorld()->SpawnActor<ADrgBaseCharacter>(
				AICharacterClass,
				SpawnLocation,
				FRotator::ZeroRotator
			);
			InActiveAIPool.Add(SpawnedAI);
		}
	}
}

bool ADrgSpawnAI::FindSafeRandomPointInNav(FVector& ResultLocation)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return false;

	for (int32 i = 0; i < MaxTryCount; i++)
	{
		float RandomDistance = FMath::RandRange(MinDistance, MaxDistance);
		float RandomAngle = FMath::RandRange(0.f, 2 * PI);
		FVector RandomOffset = FVector(
			RandomDistance * FMath::Cos(RandomAngle),
			RandomDistance * FMath::Sin(RandomAngle),
			0.f
		);

		FVector TestLocation = GetActorLocation() + RandomOffset;
		FNavLocation NavLocation;
		bool bIsFound = NavSys->ProjectPointToNavigation(TestLocation, NavLocation);
		if (!bIsFound) continue;

		ResultLocation = NavLocation.Location;
		return true;
	}
	return false;
}

struct FDrgWaveTableRow ADrgSpawnAI::GetCurrentWaveDataRow(int32 WaveNumber)
{
	if (!WaveDataTable) return FDrgWaveTableRow{};

	TArray<FDrgWaveTableRow*> AllRows;
	static const FString ContextString(TEXT("GetRandomAICharacterDataContext"));
	WaveDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.Num() == 0) return FDrgWaveTableRow{};

	FDrgWaveTableRow* CurrentWaveRow = nullptr;
	for (FDrgWaveTableRow* Row : AllRows)
	{
		if (Row && Row->WaveNumber == WaveNumber)
		{
			CurrentWaveRow = Row;
			break;
		}
	}
	if (!CurrentWaveRow)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("DrgSpawnAI::WaveNumber %d에 해당하는 열을 찾을 수 없습니다. [해결 방법: DT_Wave에 해당 웨이브 정보를 입력해주세요]"), WaveNumber);
		return FDrgWaveTableRow{};
	}
	return *CurrentWaveRow;
}

UDrgCharacterData* ADrgSpawnAI::GetRandomAICharacterData(FDrgWaveTableRow CurrentWaveRow)
{
	float TotalChance = 0;
	for (auto& p : CurrentWaveRow.AIData)
	{
		TotalChance += p.Value;
	}

	const float RandomValue = FMath::FRandRange(0.f, TotalChance);
	float AccumulatedChance = 0;
	for (auto& p : CurrentWaveRow.AIData)
	{
		AccumulatedChance += p.Value;
		if (RandomValue <= AccumulatedChance)
		{
			return p.Key;
		}
	}

	return nullptr;
}

void ADrgSpawnAI::SpawnAIFromPool(int32 WaveNumber)
{
	FDrgWaveTableRow CurrentWaveRow = GetCurrentWaveDataRow(WaveNumber);
	if (!CurrentWaveRow.AIData.Num()) return;
	FVector SpawnLocation;
	if (FindSafeRandomPointInNav(SpawnLocation))
	{
		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1.f));
		ADrgBaseCharacter* SpawnedAI = GetWorld()->SpawnActorDeferred<ADrgBaseCharacter>(
			AICharacterClass,
			SpawnTransform
		);
		SpawnedAI->SetCharacterData(GetRandomAICharacterData(CurrentWaveRow));
		UGameplayStatics::FinishSpawningActor(SpawnedAI, SpawnTransform);
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT("DrgSpawnAI::SpawnLocation을 찾을 수 없습니다."));
	}
}
