// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrgSpawnAI.generated.h"

UCLASS()
class DRG_API ADrgSpawnAI : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADrgSpawnAI();

	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void SetCurrentWaveNumber(int32 NewWaveNumber);
	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void SetNextWave();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	TObjectPtr<UDataTable> WaveDataTable;

protected:
	UPROPERTY(editAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	TSubclassOf<class ADrgAICharacter> AICharacterClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	int32 TotalSpawnCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	float MinDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	float MaxDistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drg|Spawning")
	TArray<class ADrgAICharacter*> ActiveAIPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drg|Spawning")
	TArray<class ADrgAICharacter*> InActiveAIPool;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning", meta = (ClampMin = "10"))
	int32 MaxTryCount = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drg|Spawning")
	int32 CurrentWaveNumber = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	int32 CurrentSpawnCount = 0;

	FTimerHandle SpawnTimerHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 레벨 시작 시 AI 캐릭터 미리 스폰해서 InActiveAIPool에 추가
	// 이 이상 AI 캐릭터 스폰하지 않음
	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void InitializePool();
	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void ReturnAIToPool(class ADrgAICharacter* DeadAI);
	// 현재 웨이브 스폰 타이머 시작. SpawnAILoop 반복 실행
	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void StartSpawnTimer();
	// 현재 웨이브 스폰 타이머 종료
	UFUNCTION(BlueprintCallable, Category = "Drg|Spawning")
	void StopSpawnTimer();

	// Nav Mesh 내에 안전한 스폰 위치 
	bool FindSafeRandomPointInNav(FVector& ResultLocation);
	// 데이터테이블 내 현재 웨이브 데이터 행 반환
	struct FDrgWaveTableRow* GetCurrentWaveDataRow(int32 WaveNumber);
	// 현재 웨이브에 해당하는 AI 데이터에셋 랜덤하게 반환
	TObjectPtr<class UDrgCharacterData> GetRandomAICharacterData();
	// 데이터에셋 적용하여 안전한 위치에 스폰
	TObjectPtr<class ADrgAICharacter> SpawnAIFromPool();
	// 스폰 후 ActiveAIPool에 추가. 타이머에서 실행될 함수
	void SpawnAILoop();

	// 웨이브 타이머 (초) / 디폴트 60초 = 1분
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Spawning")
	float WaveChangeTime = 60.0f;
	FTimerHandle NextWaveTimerHandle;
};
