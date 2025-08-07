// Fill out your copyright notice in the Description page of Project Settings.

#include "DrgSpawnAI.h"

#include "DrgAIController.h"
#include "NavigationSystem.h"
#include "DrgWaveTableRow.h"
#include "Components/CapsuleComponent.h"
#include "Drg/AI/DrgAICharacter.h"
#include "Drg/Character/Data/DrgCharacterData.h"
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

void ADrgSpawnAI::SetCurrentWaveNumber(int32 NewWaveNumber)
{
	CurrentWaveNumber = NewWaveNumber;
	if (bIsSpawnTimerRunning)
	{
		StopSpawnTimer();
	}
	StartSpawnTimer();
}

void ADrgSpawnAI::InitializePool()
{
	for (int32 i = 0; i < TotalSpawnCount; i++)
	{
		FVector SpawnLocation;
		if (FindSafeRandomPointInNav(SpawnLocation))
		{
			ADrgAICharacter* SpawnedAI = GetWorld()->SpawnActor<ADrgAICharacter>(
				AICharacterClass,
				SpawnLocation,
				FRotator::ZeroRotator
			);
			InActiveAIPool.Add(SpawnedAI);
			SpawnedAI->OnAIDied.AddDynamic(this, &ADrgSpawnAI::ReturnAIToPool);
		}
	}
}

void ADrgSpawnAI::ReturnAIToPool(class ADrgAICharacter* DeadAI)
{
	if (ActiveAIPool.Contains(DeadAI))
	{
		ActiveAIPool.Remove(DeadAI);
		InActiveAIPool.Add(DeadAI);
		CurrentSpawnCount--;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("풀에 AI 캐릭터를 반환하려 했지만, Active 풀에서 찾을 수 없습니다. 풀링 로직에 오류가 있을 수 있습니다. 대상 캐릭터: %s"),
		       *DeadAI->GetName());
	}
}

void ADrgSpawnAI::StartSpawnTimer()
{
	CurrentSpawnCount = 0;
	CurrentWaveRow = GetCurrentWaveDataRow(CurrentWaveNumber);
	if (!CurrentWaveRow) return;

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

	bIsSpawnTimerRunning = true;
}

void ADrgSpawnAI::StopSpawnTimer()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
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
			FVector Start = NavLocation.Location + FVector(0, 0, 1000.0f);
			FVector End = NavLocation.Location - FVector(0, 0, 1000.0f);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = false;

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility,
				QueryParams
			);

			if (bHit && HitResult.bBlockingHit)
			{
				ResultLocation = HitResult.Location;
				return true;
			}
		}
		return true;
	}
	return false;
}

FDrgWaveTableRow* ADrgSpawnAI::GetCurrentWaveDataRow(int32 WaveNumber)
{
	if (!WaveDataTable) return nullptr;

	TArray<FDrgWaveTableRow*> AllRows;
	static const FString ContextString(TEXT("GetRandomAICharacterDataContext"));
	WaveDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.Num() == 0) return nullptr;

	FDrgWaveTableRow* CurrentRow = nullptr;
	for (FDrgWaveTableRow* Row : AllRows)
	{
		if (Row && Row->WaveNumber == WaveNumber)
		{
			CurrentRow = Row;
			break;
		}
	}
	if (!CurrentRow)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("DrgSpawnAI::WaveNumber %d에 해당하는 열을 찾을 수 없습니다. [해결 방법: DT_Wave에 해당 웨이브 정보를 입력해주세요]"), WaveNumber);
		check(CurrentRow!=nullptr);
		return nullptr;
	}
	return CurrentRow;
}

TObjectPtr<UDrgCharacterData> ADrgSpawnAI::GetRandomAICharacterData()
{
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

ADrgAICharacter* ADrgSpawnAI::SpawnAIFromPool()
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
	for (int32 i = 0; i < CurrentWaveRow->SpawnCount; i++)
	{
		if (CurrentSpawnCount >= CurrentWaveRow->MaxSpawnCount)
		{
			break;
		}
		ADrgAICharacter* SpawnedAI = SpawnAIFromPool();
		if (SpawnedAI)
		{
			CurrentSpawnCount++;
		}
	}
}
