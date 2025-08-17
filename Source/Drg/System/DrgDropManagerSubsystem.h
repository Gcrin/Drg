// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Items/Data/DropSystemTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "DrgDropManagerSubsystem.generated.h"

class AInstancedPickupManager;
class UDropRegistryDataAsset;
/**
 * @brief 월드에 단 하나만 존재하며 모든 아이템 드롭 로직을 중앙에서 관리하는 서브시스템.
 * 액터 사망 메시지를 수신하여, 드롭 레지스트리를 기반으로 적절한 아이템을 스폰하는 책임을 가짐.
 */
UCLASS()
class DRG_API UDrgDropManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** UWorldSubsystem의 초기화 함수. 레벨 시작 시 호출. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** UWorldSubsystem의 해제 함수. 레벨 종료 시 호출. */
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	/**
	 * @brief GameplayMessageSubsystem에 등록되어 'Event.Actor.Death' 메시지를 수신하는 콜백 함수.
	 * @param Channel 메시지가 방송된 채널 태그.
	 * @param Message 방송된 실제 메시지 데이터.
	 */
	void OnActorDeathMessageReceived(FGameplayTag Channel, const FDrgActorDeathMessage& Message);

	/**
	 * @brief 드롭 정보를 바탕으로 실제 픽업 액터를 월드에 스폰하는 헬퍼 함수.
	 * @param DropInfo 스폰할 아이템의 정보 (클래스, 개수 등).
	 * @param Location 아이템이 스폰될 월드 위치.
	 * @param World 아이템을 스폰할 월드 객체.
	 */
	void SpawnItems(const FDrgDropItemInfo& DropInfo, const FVector& Location, UWorld* World);

	/**
	 * @brief 게임의 모든 드롭 규칙을 담는 중앙 카탈로그 데이터 애셋.
	 * Initialize 시 로드되어 메모리에 유지.
	 */
	UPROPERTY()
	TObjectPtr<UDropRegistryDataAsset> DropRegistryDataAsset;

	// GameplayMessageSubsystem에 등록된 리스너의 핸들. Deinitialize 시 리스너 해제에 사용.
	FGameplayMessageListenerHandle ListenerHandle;

	/**
	 * @brief GameplayMessageSubsystem에 대한 약한 참조 포인터.
	 * 서브시스템을 캐싱하여 반복적인 조회를 피하고, 종료 시 안전하게 접근하기 위해 사용.
	 */
	TWeakObjectPtr<UGameplayMessageSubsystem> CachedMessageSubsystem;

	UPROPERTY()
	TMap<TObjectPtr<UDrgPickupDataAsset>, TObjectPtr<AInstancedPickupManager>> InstancedManagers;
};
